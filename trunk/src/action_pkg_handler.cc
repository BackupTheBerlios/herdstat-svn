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

#include <iomanip>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <map>
#include <vector>
#include <memory>
#include <string>
#include <cstdlib>

#include "metadatas.hh"
#include "metadata_xml_handler.hh"
#include "herds_xml_handler.hh"
#include "options.hh"
#include "util.hh"
#include "formatter.hh"
#include "exceptions.hh"
#include "action_pkg_handler.hh"

/*
 * Check given herds.xml container for the specified developer
 */

bool
dev_exists(herds_T &herds_xml, std::string &dev)
{
    for (herds_T::iterator h = herds_xml.begin() ; h != herds_xml.end() ; ++h)
    {
        herd_T::iterator d = herds_xml[h->first]->find(dev + "@gentoo.org");
        if (d != herds_xml[h->first]->end())
            return true;
    }

    return false;
}

std::string
dev_name(herds_T &herds_xml, std::string &dev)
{
    for (herds_T::iterator h = herds_xml.begin() ; h != herds_xml.end() ; ++h)
    {
        herd_T::iterator d = herds_xml[h->first]->find(dev + "@gentoo.org");
        if (d != herds_xml[h->first]->end())
            return d->second->name;
    }

    return "";
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
    util::timer_T t;
    util::progress_T progress;
    std::map<std::string, std::string>::size_type size = 0;
    std::vector<std::string> not_found;

    std::ostream *stream = optget("outstream", std::ostream *);

    /* this code takes long enough as it is... no need for
     * a zillion calls to optget inside loops */
    bool quiet = optget("quiet", bool);
    bool verbose = optget("verbose", bool);
    bool timer = optget("timer", bool);
    bool count = optget("count", bool);
    bool debug = optget("debug", bool);
    bool dev = optget("dev", bool);
    bool status = not quiet and not debug;

    /* set format attributes */
    formatter_T output;
    output.set_maxlabel(16);
    output.set_maxdata(optget("maxcol", std::size_t) - output.maxlabel());
    output.set_attrs();

    /* action_pkg_handler doesn't support the all target */
    if (optget("all", bool))
    {
        std::cerr << "Package action handler does not support the 'all' target."
            << std::endl;
        return EXIT_FAILURE;
    }

    /* check PORTDIR */
    if (not util::is_dir(optget("portdir", std::string)))
	throw bad_fileobject_E(optget("portdir", std::string));

    /* check opts */
    std::vector<std::string> foundopts(opts);
    std::vector<std::string>::iterator i;
    for (i = opts.begin() ; i != opts.end() ; ++i)
    {
        if ((dev and not dev_exists(herds_xml, *i)) or
            (not dev and not herds_xml.exists(*i)))
        {
            foundopts.erase(std::remove(foundopts.begin(),
                foundopts.end(), *i), foundopts.end());
        }
    }

    /* our list of metadata.xml's */
    metadatas_T metadatas(optget("portdir", std::string));

    if (status)
    {
        *stream << "Parsing metadata.xml's: ";
        if (not foundopts.empty())
            progress.start(foundopts.size() * metadatas.size());
        else
            progress.start(opts.size());
    }

    /* for each specified herd/dev... */
    std::vector<std::string>::size_type n = 1;
    for (i = opts.begin() ; i != opts.end() ; ++i, ++n)
    {
        dev_attrs_T attr;
        std::map<std::string, std::string> pkgs;

        if (dev and not dev_exists(herds_xml, *i))
        {
            if (foundopts.empty())
                ++progress;

            if (opts.size() == 1)
            {
                if (not quiet)
                    std::cerr << std::endl << "Developer '" << *i
                        << "' doesn't seem to exist."; 
                throw dev_E();
            }

            not_found.push_back(*i);
            continue;
        }
        else if (not dev and not herds_xml.exists(*i))
        {
            if (foundopts.empty())
                ++progress;

            if (opts.size() == 1)
            {
                if (not quiet)
                    std::cerr << std::endl << "Herd '" << *i
                        << "' doesn't seem to exist.";
                throw herd_E();
            }

            not_found.push_back(*i);
            continue;
        }

        if (timer)
            t.start();

        /* for each metadata.xml... */
        metadatas_T::iterator m;
        for (m = metadatas.begin() ; m != metadatas.end() ; ++m)
        {
            if (status)
                ++progress;

            /* unfortunately, we still have to do a sanity check in the event
             * that the user has sync'd, a metadata.xml has been removed, and
             * the cache not yet updated ; otherwise we'll get a parser error */
            if (not util::is_file(*m))
                continue;

            try
            {
                bool found = false;

                std::auto_ptr<MetadataXMLHandler_T> handler(new
                    MetadataXMLHandler_T());
                XMLParser_T parser(&(*handler));

                /* parse metadata.xml */
                parser.parse(*m);

                if (dev)
                {
                    /* search the metadata for our dev */
                    herd_T::iterator d = handler->devs.find(*i + "@gentoo.org");
                    if (d == handler->devs.end())
                        continue;
                    else
                    {
                        bool copy = false;
                        if (not optget("with-herd", std::string).empty())
                        {
                            if (std::find(handler->herds.begin(),
                                handler->herds.end(),
                                optget("with-herd", std::string)) !=
                                handler->herds.end())
                            {
                                copy = true;
                            }
                        }
                        else
                            copy = true;

                        if (copy)
                        {
                            found = true;
                            std::copy(d->second->begin(), d->second->end(),
                                attr.begin());
                            attr.name = dev_name(herds_xml, *i);
                            attr.role = d->second->role;
                        }
                    }
                }
                else
                {
                    /* search the metadata for our herd */
                    if (std::find(handler->herds.begin(),
                        handler->herds.end(), *i) == handler->herds.end())
                        continue;
                    else
                        found = true;
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
        
        if (timer)
            t.stop();

        size += pkgs.size();

        if ((n == 1) and status)
            output.endl();

        if (not quiet)
        {
            if (dev)
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
                    output("Description", util::tidy_whitespace(herds_xml[*i]->desc));
            }

            if (pkgs.empty())
                output("Packages(0)", "none");
            /* display first package on same line */
            else if (verbose)
                output(util::sprintf("Packages(%d)", pkgs.size()),
                    color[blue] + pkgs.begin()->first + color[none]);
            else
                output(util::sprintf("Packages(%d)", pkgs.size()),
                    pkgs.begin()->first);
        }
        else if (not pkgs.empty() and not count)
            output("", pkgs.begin()->first);

        /* display the category/package */
        std::map<std::string, std::string>::iterator p =
            ( pkgs.empty() ? pkgs.begin() : ++(pkgs.begin()) );
        std::map<std::string, std::string>::size_type pn = 1;
        for ( ; p != pkgs.end() ; ++p, ++pn)
        {
            std::string longdesc;
            
            if (not p->second.empty())
                longdesc = util::tidy_whitespace(p->second);

            if ((verbose and not quiet) and not longdesc.empty())
            {
                if (output.size() > 1 and output.peek() != "")
                    output.endl();

                if (optget("color", bool))
                    output("", color[blue] + p->first + color[none]);
                else
                    output("", p->first);

                output("", longdesc);
                util::debug_msg("longdesc(%s): '%s'", p->first.c_str(),
                    longdesc.c_str());

                if (pn != pkgs.size())
                    output.endl();
            }
            else if (verbose and not quiet)
            {
                if (optget("color", bool))
                    output("", color[blue] + p->first + color[none]);
                else
                    output("", p->first);
            }
            else if (not count)
                output("", p->first);
        }

        /* only skip a line if we're not on the last one */
        if (not count and n != foundopts.size())
            if (not quiet or (quiet and pkgs.size() > 0))
                output.endl();
    }

    if (count)
        output("", util::sprintf("%d", size));

    output.flush(*stream);

    for (i = not_found.begin() ; i != not_found.end() ; ++i)
    {
        if (dev)
            *stream << std::endl << "Developer '" << *i <<
                "' does not seem to exist.";
        else
            *stream << std::endl << "Herd '" << *i <<
                "' does not seem to exist.";
    }

    if (not not_found.empty())
        *stream << std::endl;

    if (timer)
    {
        *stream << std::endl << "Took " << t.elapsed() << "ms to parse "
            << metadatas.size() << " metadata.xml's ("
            << std::setprecision(4)
            << (static_cast<float>(t.elapsed()) / metadatas.size())
            << " ms/metadata.xml)." << std::endl;
    }
    else if (verbose and not quiet)
    {
        *stream << std::endl
            << "Parsed " << metadatas.size() << " metadata.xml's." << std::endl;
    }

    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
