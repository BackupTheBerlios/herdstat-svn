/*
 * herdstat -- src/action_pkg_handler.cc
 * $Id$
 * Copyright (c) 2005 Aaron Walker <ka0ttic at gentoo.org>
 *
 * This file is part of herdstat.
 *
 * herdstat is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * herdstat is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * herdstat; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 325, Boston, MA  02111-1257  USA
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <map>
#include <vector>
#include <memory>
#include <string>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

#include "metadata_xml_handler.hh"
#include "herds_xml_handler.hh"
#include "options.hh"
#include "util.hh"
#include "formatter.hh"
#include "exceptions.hh"
#include "action_pkg_handler.hh"

/*
 * Retrieve a list of all categories from PORTDIR/profiles/categories
 */

std::vector<std::string>
get_categories(const std::string &portdir)
{
    std::string catfile = portdir + "/profiles/categories";
    std::vector<std::string> categories;

    std::auto_ptr<std::istream> f(new std::ifstream(catfile.c_str()));
    if (not (*f))
        throw bad_fileobject_E("Failed to open '%s': %s", catfile.c_str(),
            strerror(errno));

    std::string s;
    while (std::getline(*f, s))
        categories.push_back(s);

    return categories;
}

/*
 * Return a list of _all_ metadata.xml files.
 */

std::vector<std::string>
get_metadatas(const std::string &portdir)
{
    std::vector<std::string> metadatas;
    std::vector<std::string> categories = get_categories(portdir);
    std::vector<std::string>::iterator c;
    
    for (c = categories.begin() ; c != categories.end() ; ++c)
    {
        std::string cat = portdir + "/" + (*c);

        /* open category */
        DIR *dir = opendir(cat.c_str());
        if (not dir)
            continue;
        
        util::debug_msg("opened directory %s", cat.c_str());

        struct dirent *d;
        while ((d = readdir(dir)))
        {
            /* skip anything starting with a '.' */
            if (std::strncmp(d->d_name, ".", 1) == 0)
                continue;

            std::string metadata = cat + "/" + d->d_name + "/metadata.xml";
            if (util::is_file(metadata))
                metadatas.push_back(metadata);
        }
        closedir(dir);
    }
    return metadatas;
}

bool
dev_exists(herds_T &herds_xml, std::string dev)
{
    for (herds_T::iterator h = herds_xml.begin() ; h != herds_xml.end() ; ++h)
    {
        herd_T::iterator d = herds_xml[h->first]->find(dev + "@gentoo.org");
        if (d != herds_xml[h->first]->end())
            return true;
    }
    return false;
}

/*
 * Given a list of herds/devs, determine all packages belonging
 * to each herd/dev. For reliability reasons, every metadata.xml
 * in the tree is parsed.
 */

int
action_pkg_handler_T::operator() (herds_T &herds_xml,
                                  std::vector<std::string> &opts)
{
    util::color_map_T color;
    util::timer_T timer;
    std::ostream *stream = optget("outstream", std::ostream *);

    /* set format attributes */
    formatter_T output;
    output.set_maxlabel(16);
    output.set_maxdata(optget("maxcol", std::size_t) - output.maxlabel());
    output.set_attrs();

    if (optget("all", bool))
    {
        std::cerr << "Package action handler does not support the 'all' target."
            << std::endl;
        return EXIT_FAILURE;
    }

    /* before trying to get a list of metadatas,
     * see if the herd/dev even exists */
    if (opts.size() == 1 and optget("dev", bool))
    {
        if (not dev_exists(herds_xml, opts[0]))
        {
            if (not optget("quiet", bool))
                std::cerr << "Developer '" << opts[0]
                    << "' doesn't seem to exist." << std::endl;
            throw dev_E();
        }
    }
    else if (opts.size() == 1)
    {
        if (not herds_xml.exists(opts[0]))
        {
            if (not optget("quiet", bool))
                std::cerr << "Herd '" << opts[0] << "' doesn't seem to exist."
                    << std::endl;
            throw herd_E();
        }
    }

    if (not optget("quiet", bool))
        *stream << "Parsing metadata.xml's (this may take a while)..."
            << std::endl;

    /* get a list of every metadata.xml */
    std::vector<std::string> metadatas;
    try
    {
	/* PORTDIR */
	optset("portdir", std::string, util::portdir());
        char *result = getenv("PORTDIR");
	if (result)
	    optset("portdir", std::string, result);

        /* make sure it exists */
	if (not util::is_dir(optget("portdir", std::string)))
	    throw bad_fileobject_E("PORTDIR '%s' does not exist.",
		optget("portdir", std::string).c_str());

        if (optget("timer", bool))
            timer.start();

        /* get a list of all metadata.xml's */
        metadatas = get_metadatas(optget("portdir", std::string));

        if (optget("timer", bool))
        {
            timer.stop();
            util::debug_msg("Took %ldms to get a list of every metadata.xml in the tree.",
                timer.elapsed());
        }
    }
    catch (const bad_fileobject_E &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    if (not optget("quiet", bool))
        output.endl();

    std::map<std::string, std::string>::size_type size = 0;

    /* for each specified herd/dev... */
    std::vector<std::string>::iterator i;
    std::vector<std::string>::size_type n = 1;
    for (i = opts.begin() ; i != opts.end() ; ++i, ++n)
    {
        dev_attrs_T attr;
        std::map<std::string, std::string> pkgs;

        if (optget("dev", bool))
        {
            if (not dev_exists(herds_xml, *i))
            {
                if (not optget("quiet", bool))
                    std::cerr << std::endl << "Developer '" << *i
                        << "' doesn't seem to exist in herds.xml." << std::endl;

                if (opts.size() > 1)
                    continue;
                else
                    throw dev_E();
            }
        }
        else
        {
            if (not herds_xml.exists(*i))
            {
                if (not optget("quiet", bool))
                    std::cerr << std::endl << "Herd '" << *i
                        << "' doesn't seem to exist in herds.xml." << std::endl;

                if (opts.size() > 1)
                    continue;
                else
                    throw herd_E();
            }
        }

        if (optget("timer", bool))
            timer.start();

        /* for each metadata.xml... */
        std::vector<std::string>::iterator m;
        for (m = metadatas.begin() ; m != metadatas.end() ; ++m)
        {
            try
            {
                bool found = false;

                std::auto_ptr<MetadataXMLHandler_T> handler(new
                    MetadataXMLHandler_T());
                XMLParser_T parser(&(*handler));

                /* parse metadata.xml */
                parser.parse(*m);

                if (optget("dev", bool))
                {
                    herd_T::iterator d = handler->devs.find(*i + "@gentoo.org");
                    if (d == handler->devs.end())
                        continue;
                    else
                    {
                        found = true;
                        std::copy(d->second->begin(), d->second->end(), attr.begin());
                        attr.name = d->second->name;
                        attr.role = d->second->role;
                    }
                }
                else
                {
                    /* search the metadata for our herd */
                    if (std::find(handler->herds.begin(),
                        handler->herds.end(), *i) != handler->herds.end())
                        found = true;
                    else
                        continue;
                }

                if (found)
                {
                    /* get category/package from absolute path */
                    std::string cat_and_pkg =
                        (*m).substr(optget("portdir", std::string).size() + 1);
                    std::string::size_type pos = cat_and_pkg.find("/metadata.xml");
                    if (pos != std::string::npos)
                        cat_and_pkg = cat_and_pkg.substr(0, pos);

                    util::debug_msg("Match found in %s.", (*m).c_str());

                    pkgs[cat_and_pkg] = handler->longdesc;
                }
            }
            catch (const XMLParser_E &e)
            {
                std::cerr << "Error parsing '" << e.file()
                    << "': " << e.error() << std::endl;
                return EXIT_FAILURE;
            }
        }

        size += pkgs.size();

        if (optget("timer", bool))
            timer.stop();

        if (not optget("quiet", bool))
        {
            if (optget("dev", bool))
            {
                if (attr.name.empty())
                    output("Developer", *i);
                else
                    output("Developer",
                        attr.name + " (" + (*i) + ")");

                output("Email", *i + "@gentoo.org");
            }
            else
            {
                output("Herd", *i);
                if (not herds_xml[*i]->mail.empty())
                    output("Email", herds_xml[*i]->mail);
                if (not herds_xml[*i]->desc.empty())
                    output("Description", herds_xml[*i]->desc);
            }

            output(util::sprintf("Packages(%d)", pkgs.size()), "");
        }

        /* display the category/package */
        std::map<std::string, std::string>::iterator p;
        std::map<std::string, std::string>::size_type pn = 1;
        for (p = pkgs.begin() ; p != pkgs.end() ; ++p, ++pn)
        {
            if ((optget("verbose", bool) and not optget("quiet", bool))
                and not p->second.empty())
            {
                if (output.size() > 1 and output.peek() != "")
                    output.endl();

                if (optget("color", bool))
                    output("", color[blue] + p->first + color[none]);
                else
                    output("", p->first);

                output("", p->second);
                util::debug_msg("longdesc(%s): '%s'", p->first.c_str(),
                    p->second.c_str());

                if (pn != pkgs.size())
                    output.endl();
            }
            else if (optget("verbose", bool) and not optget("quiet", bool))
            {
                if (optget("color", bool))
                    output("", color[blue] + p->first + color[none]);
                else
                    output("", p->first);
            }
            else if (not optget("count", bool))
                output("", p->first);
        }

        /* only skip a line if we're not on the last one */
        if (not optget("count", bool) and n != opts.size())
        {
            if ((not optget("quiet", bool)) or
                (optget("quiet", bool) and pkgs.size() > 0))
                    output.endl();
        }
    }

    if (optget("count", bool))
        output("", util::sprintf("%d", size));

    output.flush(*stream);

    if (optget("timer", bool))
    {
        *stream << std::endl << "Took " << timer.elapsed() << "ms to parse "
            << metadatas.size() << " metadata.xml's ("
            << std::setprecision(4)
            << (static_cast<float>(timer.elapsed()) / metadatas.size())
            << " ms/metadata.xml)." << std::endl;
    }
    else if (optget("verbose", bool) and not optget("quiet", bool))
    {
        *stream << std::endl
            << "Parsed " << metadatas.size() << " metadata.xml's." << std::endl;
    }

    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
