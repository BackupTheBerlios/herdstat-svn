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
#include <fstream>
#include <algorithm>
#include <map>
#include <vector>
#include <utility>
#include <memory>
#include <string>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <xmlwrapp/init.h>

#include "metadata.hh"
#include "herds.hh"
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

    if ((*f))
    {
        while (not f->eof())
        {
            std::string s;
            if (std::getline(*f, s))
                categories.push_back(s);
        }
    }
    else
        throw bad_fileobject_E("Failed to open '%s': %s", catfile.c_str(),
            strerror(errno));

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

        /* make sure it's valid before trying to open it.
         * unfortunately profiles/categories isn't always 100% up-to-date.  */
        if (not util::is_dir(cat))
            continue;

        /* open category */
        DIR *dir = opendir(cat.c_str());
        if (not dir)
        {
            throw bad_fileobject_E("Failed to open directory '%s': %s",
                cat.c_str(), strerror(errno));
        }
        
        util::debug_msg("opened directory %s", cat.c_str());

        struct dirent *d;
        while ((d = readdir(dir)))
        {
            std::string metadata = cat + "/" + d->d_name + "/metadata.xml";
            if (util::is_file(metadata))
                metadatas.push_back(metadata);
        }

        closedir(dir);
    }

    return metadatas;
}


/*
 * Given a list of herds, determine all packages in each herd.
 * For reliability reasons, every metadata.xml in the tree is
 * parsed.
 */

int
action_pkg_handler_T::operator() (HerdsXMLHandler_T *hhandler,
                                  std::vector<std::string> &herds)
{
    options_T options;
    util::color_map_T color;
    util::timer_T timer;
    std::vector<std::string> metadatas;

    try
    {
        /* FIXME! this is icky.
         * if the default portdir doesn't exist, fall back to portageq */
        if (not util::is_dir(options.portdir()))
        {
            char line[PATH_MAX+1];
            FILE *p = popen("portageq portdir", "r");
            if (p)
            {
                if (std::fgets(line, sizeof(line) - 1, p) != NULL)
                    options.set_portdir(line);
                pclose(p);
            }
            else
                throw bad_fileobject_E("Failed to determine PORTDIR!");
        }

        metadatas = get_metadatas(options.portdir());
    }
    catch (const bad_fileobject_E &e)
    {
        std::cerr << color[red] << e.what() << color[none] << std::endl;
        return EXIT_FAILURE;
    }
    
    /* set format attributes */
    fm::formatter_T output;
    output.set_maxlabel(16);
    output.set_maxdata(options.maxcol() - output.maxlabel());
    output.set_colors(true);
    output.set_labelcolor(color[green]);
    output.set_attrs();

    /* for each specified herd... */
    std::vector<std::string>::iterator herd;
    std::vector<std::string>::size_type n = 0;
    for (herd = herds.begin() ; herd != herds.end() ; ++herd)
    {
        std::map<std::string, std::string> pkgs;

        /* does the herd exist? */
        if (not hhandler->exists(*herd))
        {
            std::cerr << color[red] << "Herd '" << *herd
                << "' doesn't seem to exist." << color[none] << std::endl;

            /* if the user specified more than one herd, then just print
             * the error and keep going; otherwise, we want to exit with
             * an error code
             */
            if (herds.size() > 1)
            {
                std::cerr << std::endl;
                continue;
            }
            else
                throw herd_E();
        }

        if (options.timer())
            timer.start();

        /* for each metadata.xml... */
        std::vector<std::string>::iterator m;
        for (m = metadatas.begin() ; m != metadatas.end() ; ++m)
        {
            try
            {
                std::auto_ptr<MetadataXMLHandler_T> mhandler(new
                    MetadataXMLHandler_T());
                XMLParser_T parser(&(*mhandler));

                /* parse metadata.xml */
                parser.parse(*m);

                /* search the metadata for our herd */
                std::vector<std::string>::iterator pos;
                pos = std::find(mhandler->herds.begin(),
                    mhandler->herds.end(), *herd);

                /* is it there? */
                if (pos == mhandler->herds.end())
                    continue;
                else
                {
                    /* get category/package from absolute path */
                    std::string cat_and_pkg = (*m).substr(options.portdir().size() + 1);
                    std::string::size_type pos = cat_and_pkg.find("/metadata.xml");
                    if (pos != std::string::npos)
                        cat_and_pkg = cat_and_pkg.substr(0, pos);

                    util::debug_msg("Match found in %s.", (*m).c_str());

                    pkgs.insert(std::make_pair(cat_and_pkg, mhandler->longdesc));
                }
            }
            catch (const XMLParser_E &e)
            {
                std::cerr << color[red] << "Error parsing '" << e.file()
                    << "': " << e.error() << color[none] << std::endl;
                return EXIT_FAILURE;
            }
        }

        if (options.timer())
            timer.stop();

        /* we now have the list of packages that correspond to the herd */

        if (not options.quiet())
        {
            output.append("Herd", *herd);
            output.append("Description", hhandler->descs[*herd]);
            output.append(util::sprintf("Packages(%d)", pkgs.size()), "");
        }

        /* display the category/package */
        std::map<std::string, std::string>::iterator p;
        for (p = pkgs.begin() ; p != pkgs.end() ; ++p)
        {
            if (options.quiet())
                std::cout << p->first << std::endl;
            else
            {
                /* TODO: the below code works, but until we figure out a way
                 * to cleanup all the whitespace, the output looks like shit */

//                if (options.verbose() and not p->second.empty())
//                {
//                    output.append("", color[blue] + p->first + color[none]);
//                    output.append("", p->second);
//                }
//                else
                    output.append("", p->first);
            }
        }

        /* only skip a line if we're not on the last one */
        if (++n != herds.size())
            output.endl();
    }

    if (not options.quiet())
        output.flush(std::cout);

    if (options.timer())
    {
        std::cout << std::endl << "Took " << timer.elapsed()
            << "ms to parse every metadata.xml in the tree." << std::endl;
    }

    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
