/*
 * herdstat -- src/action_meta_handler.cc
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
#include "action_meta_handler.hh"

/*
 * Find all packages with the given name. Return a list of each
 * one found in cat/pkg form.
 */

std::vector<std::string>
get_possibles(const std::string &pkg)
{
    std::vector<std::string> pkgs;
    std::string portdir = optget("portdir", std::string);

    /* if category was specified, just check for existence */
    std::string::size_type pos = pkg.find('/');
    if (pos != std::string::npos)
    {
        if (util::is_dir(portdir + "/" + pkg))
            pkgs.push_back(pkg);
        return pkgs;
    }

    std::vector<std::string> categories = util::get_categories(portdir);
    std::vector<std::string>::iterator c;
    for (c = categories.begin() ; c != categories.end() ; ++c)
    {
        std::string path = portdir + "/" + (*c);

        if (*c == pkg)
        {
            pkgs.push_back(*c);
            return pkgs;
        }

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
action_meta_handler_T::operator() (herds_T &herds_xml,
                                    std::vector<std::string> &pkgs)
{
    util::color_map_T color;
    formatter_T output;
    output.set_maxlabel(16);
    output.set_maxdata(optget("maxcol", std::size_t) - output.maxlabel());
    output.set_quiet(false);
    output.set_attrs();

    /* we dont care about these */
    optset("verbose", bool, false);
    optset("timer", bool, false);

    if (optget("all", bool))
    {
        std::cerr << "Metadata action handler does not support the 'all' target."
            << std::endl;
        return EXIT_FAILURE;
    }

    /* check PORTDIR */
    std::string portdir = util::portdir();
    optset("portdir", std::string, portdir);
    if (not util::is_dir(portdir))
        throw bad_fileobject_E(portdir);

    /* for each specified package/category... */
    std::vector<std::string>::iterator i;
    std::vector<std::string>::size_type n = 1;
    for (i = pkgs.begin() ; i != pkgs.end() ; ++i, ++n)
    {
        bool cat = false;
        herd_T devs;
        std::vector<std::string> herds;
        std::vector<std::string> possibles = get_possibles(*i);
        std::string longdesc, metadata;

        /* is there more than one package with that name? */
        if (possibles.size() > 1)
        {
            std::cerr << *i << " is ambiguous." << std::endl << std::endl
                << "Possible matches: " << std::endl;

            std::vector<std::string>::iterator p;
            for (p = possibles.begin() ; p != possibles.end() ; ++p)
                std::cerr << "   " << color[green] << *p << color[none] << std::endl;

            return EXIT_FAILURE;
        }
        else if (possibles.empty() and pkgs.size() == 1)
        {
            std::cerr << *i << " does not seem to exist." << std::endl;
            return EXIT_FAILURE;
        }
        /* or none perhaps? */
        else if (possibles.empty())
        {
            std::cerr << *i << "' does not seem to exist." << std::endl << std::endl;
            continue;
        }

        if (util::is_file(portdir + "/" + possibles.front() + "/metadata.xml"))
        {
            try
            {
                std::auto_ptr<MetadataXMLHandler_T>
                    handler(new MetadataXMLHandler_T());
                XMLParser_T parser(&(*handler));

                /* parse it */
                parser.parse(portdir + "/" + possibles.front() +
                             "/metadata.xml");

                herds = handler->herds;
                devs = handler->devs;
                longdesc = handler->longdesc;
            }
            catch (const XMLParser_E &e)
            {
                std::cerr << "Error parsing '" << e.file() << "': "
                    << e.error() << std::endl;
                return EXIT_FAILURE;
            }

            if (n != 1)
                output.endl();

            /* if no '/' exists, assume it's a category */
            if (possibles.front().find("/") == std::string::npos)
                cat = true;

            if (cat)
                output("Category", possibles.front());
            else
                output("Package", possibles.front());

            /* herds */
            if (not cat and (herds.empty() or (herds.front() == "no-herd")))
                output("Herds(0)", "none");
            else if (not herds.empty())
                output(util::sprintf("Herds(%d)", herds.size()), herds);

            /* devs */
            if (devs.size() >= 1)
                output(util::sprintf("Maintainers(%d)", devs.size()),
                    devs.keys().front());
            
            if (devs.size() > 1)
            {
                std::vector<std::string> dev_keys(devs.keys());
                std::vector<std::string>::iterator d;
                for (d = ( dev_keys.begin() + 1 ); d != dev_keys.end() ; ++d)
                    output("", *d);
            }
            else if (not cat and devs.empty())
                output("Maintainers(0)", "none");

            if (not cat)
            {
                /* HOMEPAGE */
                std::string homepage = util::get_ebuild_var(portdir,
                    possibles.front(), "HOMEPAGE");
                if (not homepage.empty())
                    output("Homepage", homepage);
            }

            /* long description */
            if (longdesc.empty())
            {
                if (not cat)
                {
                    /* ebuild's DESCRIPTION */
                    longdesc = util::get_ebuild_var(portdir, possibles.front(),
                        "DESCRIPTION");

                    if (not longdesc.empty())
                        output("Description", longdesc);
                }
            }
            else
                output("Description", util::tidy_whitespace(longdesc));
        }
        else
        {
            output("Package", possibles.front());
            output("", "No metadata.xml.");
        }
    }

    output.flush(*optget("outstream", std::ostream *));
    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
