/*
 * herdstat -- src/action_pkgto_handler.cc
 * $Id$
 * Copyright (c) 2005 Aaron Walker <ka0ttic@gentoo.org>
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

#include <ostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <unistd.h>
#include <dirent.h>

#include "herds.hh"
#include "formatter.hh"
#include "options.hh"
#include "util.hh"
#include "exceptions.hh"
#include "xmlparser.hh"
#include "metadata_xml_handler.hh"
#include "action_pkgto_handler.hh"

/*
 * Find all packages with the given name. Return a list of each
 * one found in cat/pkg form.
 */

std::vector<std::string>
get_possibles(const std::string &pkg)
{
    std::vector<std::string> pkgs;
    std::string portdir = optget("portdir", std::string);

    std::vector<std::string>::iterator c;
    std::vector<std::string> categories = util::get_categories(portdir);
    
    for (c = categories.begin() ; c != categories.end() ; ++c)
    {
        std::string path = portdir + "/" + (*c);

        /* open category */
        DIR *dir = opendir(path.c_str());
        if (not dir)
            continue;

        struct dirent *d = NULL;
        while ((d = readdir(dir)))
            if (pkg == d->d_name)
                pkgs.push_back(*c + "/" + d->d_name);

        closedir(dir);
    }

    return pkgs;
}

int
action_pkgto_handler_T::operator() (herds_T &herds_xml,
                                    std::vector<std::string> &pkgs)
{
    util::color_map_T color;

    std::ostream *stream = optget("outstream", std::ostream *);

    formatter_T output;
    output.set_maxlabel(16);
    output.set_maxdata(optget("maxcol", std::size_t) - output.maxlabel());
    output.set_attrs();

    std::string portdir = util::portdir();

    if (optget("all", bool))
    {
        std::cerr << "Package action handler does not support the 'all' target."
            << std::endl;
        return EXIT_FAILURE;
    }

    /* check PORTDIR */
    optset("portdir", std::string, portdir);
    if (not util::is_dir(portdir))
        throw bad_fileobject_E(portdir);

    /* for each specified package... */
    std::vector<std::string>::iterator i;
    std::vector<std::string>::size_type n = 1;
    for (i = pkgs.begin() ; i != pkgs.end() ; ++i, ++n)
    {
        std::vector<std::string> possibles;
        std::vector<std::string> herds;
        herd_T devs;

        /* was a category specified? */
        std::string::size_type pos = i->find("/");
        if (pos != std::string::npos)
            possibles.push_back(*i);
        else
            possibles = get_possibles(*i);

        /* is there more than one package with that name? */
        if (possibles.size() > 1)
        {
            *stream << *i << " is ambiguous.  Possible matches are:"
                << std::endl << std::endl;

            std::vector<std::string>::iterator p;
            for (p = possibles.begin() ; p != possibles.end() ; ++p)
                *stream << "  " << color[green] << *p << color[none] << std::endl;

            return EXIT_FAILURE;
        }

        /* package exists? */
        if (not util::is_dir(portdir + "/" + possibles.front()))
        {
            std::cerr << "Package '" << possibles.front()
                << "' does not seem to exist." << std::endl;
        }

        std::string metadata =
            portdir + "/" + possibles.front() + "/metadata.xml";

        output("Package", possibles.front());
        
        if (util::is_file(metadata))
        {
            try
            {
                std::auto_ptr<MetadataXMLHandler_T>
                    handler(new MetadataXMLHandler_T());
                XMLParser_T parser(&(*handler));

                /* parse it */
                parser.parse(metadata);

                herds = handler->herds;
                devs = handler->devs;
            }
            catch (const XMLParser_E &e)
            {
                std::cerr << "Error parsing '" << e.file() << "': "
                    << e.error() << std::endl;
                return EXIT_FAILURE;
            }

            if (herds.empty())
                output("Herds(0)", "None");
            else
                output(util::sprintf("Herds(%d)", herds.size()), herds);

            if (devs.empty())
                output("Maintainers(0)", "None");
            else
                output(util::sprintf("Maintainers(%d)", devs.size()),
                    devs.keys());
        }
        else
        {
            output("", "No metadata.xml");
        }

        if (n != pkgs.size())
            output.endl();
    }

    output.flush(*stream);

    if (optget("timer", bool))
        *stream << std::endl;

    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
