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
#include <utility>
#include <algorithm>
#include <iterator>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <dirent.h>

#include "common.hh"
#include "herds.hh"
#include "formatter.hh"
#include "xmlparser.hh"
#include "metadata_xml_handler.hh"
#include "action_meta_handler.hh"

/*
 * Given a vector of overlays, search them
 * for the specified package, returning a pair<overlay,package>.
 */

int
action_meta_handler_T::operator() (std::vector<std::string> &opts)
{
    util::color_map_T color;
    formatter_T output;
    std::ostream *stream = optget("outstream", std::ostream *);
    const bool quiet = optget("quiet", bool);
    bool pwd = false;

    portage::config_T config(optget("portage.config", portage::config_T));
    const std::string real_portdir(config.portdir());
    std::string portdir;

    std::vector<std::string> portdirs;

    output.set_maxlabel(16);
    output.set_maxdata(optget("maxcol", std::size_t) - output.maxlabel());
    output.set_quiet(quiet, " ");
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

    /* we can be called with 0 opts if we are currently
     * in a directory that contains a metadata.xml */
    if (opts.empty())
    {
        unsigned short depth = 0;

        /* are we in a package's directory? */
        if (portage::in_pkg_dir())
            depth = 2;
        /* not in pkgdir and metdata exists,
         * so assume we're in a category */
        else if (util::is_file("metadata.xml"))
            depth = 1;
        else
        {
            std::cerr << "You must be in a package directory or category if you" << std::endl
                << "want to run " << PACKAGE << " -m with no non-option arguments." << std::endl;
            return EXIT_FAILURE;
        }

        /* Loop, trimming each directory from the end until depth == 0 */
        std::string leftover;
        std::string path = util::getcwd();
        while (depth > 0)
        {
            std::string::size_type pos = path.rfind('/');
            if (pos != std::string::npos)
            {
                if (leftover.empty())
                    leftover = path.substr(pos + 1);
                else
                    leftover = path.substr(pos + 1) + "/" + leftover;

                path = path.substr(0, pos);
            }
            --depth;
        }

        /* now assign portdir to our path, treating the leftovers as the
         * category or category/package */
        pwd = true;
        portdir = path;
        opts.push_back(leftover);
        
        debug_msg("set portdir to '%s'", portdir.c_str());
        debug_msg("added '%s' to opts.", leftover.c_str());
    }

    /* for each specified package/category... */
    std::vector<std::string>::iterator i;
    std::vector<std::string>::size_type n = 1;
    for (i = opts.begin() ; i != opts.end() ; ++i, ++n)
    {
        bool cat = false;
        herd_T devs;
        std::vector<std::string> herds;
        std::vector<std::string>::size_type portdirn = 0;
        std::string longdesc, package;

        try
        {
            /* The only reason portdir should be set already is if opts
             * did == 0 and portdir set to $PWD */
            if (pwd)
                package = portage::find_package_in(portdir, *i);
            else
            {
                std::pair<std::string, std::string> p =
                    portage::find_package(config, *i, optget("overlay", bool));
                portdir = p.first;
                package = p.second;
            }
        }
        catch (const portage::ambiguous_pkg_E &e)
        {
            std::cerr << e.name()
                << " is ambiguous. Possible matches are: "
                << std::endl << std::endl;
            
            std::vector<std::string>::const_iterator i;
            for (i = e.packages.begin() ; i != e.packages.end() ; ++i)
            {
                if (quiet or not optget("color", bool))
                    std::cerr << *i << std::endl;
                else
                    std::cerr << color[green] << *i << color[none] << std::endl;
            }

            if (opts.size() == 1)
                return EXIT_FAILURE;
            else
                continue;
        }
        catch (const portage::nonexistent_pkg_E &e)
        {
            std::cerr << *i << " doesn't seem to exist." << std::endl;

            if (opts.size() == 1)
                return EXIT_FAILURE;
            else
                continue;
        }

        /* are we in an overlay? */
        if (portdir != real_portdir and not pwd)
        {
            /* have we already added it? */
            std::vector<std::string>::iterator pos =
                std::find(portdirs.begin(), portdirs.end(), portdir);

            /* doesn't exist, add it */
            if (pos == portdirs.end())
            {
                portdirs.push_back(portdir);
                portdirn = portdirs.size();
            }
            /* exists, find it's position */
            else
            {
                portdirn = 1;
                for (pos = portdirs.begin() ; pos != portdirs.end() ; ++pos, ++portdirn)
                    if (*pos == portdir)
                        break;
            }
        }

        /* if no '/' exists, assume it's a category */
        cat = (package.find('/') == std::string::npos);

        if (n != 1)
            output.endl();

        if (portdir == real_portdir or pwd)
            output(cat ? "Category" : "Package", package);

        /* it's in an overlay, so show a little thinggy to mark it as such */
        else if (not pwd)
            output(cat ? "Category" : "Package",
                util::sprintf("%s%s[%d]%s", package.c_str(), color[cyan].c_str(),
                portdirn, color[none].c_str()));

        /* does the metadata.xml exist? */
        if (util::is_file(portdir + "/" + package + "/metadata.xml"))
        {
            util::vars_T ebuild_vars;

            /* parse it */
            {
                std::auto_ptr<MetadataXMLHandler_T>
                    handler(new MetadataXMLHandler_T());
                XMLParser_T parser(&(*handler));

                parser.parse(portdir + "/" + package + "/metadata.xml");

                herds = handler->herds;
                devs = handler->devs;
                longdesc = handler->longdesc;
            }

            /* herds */
            if (not cat and (herds.empty() or (herds.front() == "no-herd")))
                output("Herds(0)", "none");
            else if (not herds.empty())
                output(util::sprintf("Herds(%d)", herds.size()), herds);

            /* devs */
            if (quiet)
            {
                if (devs.size() >= 1)
                    output("", devs.keys());
                else if (not cat)
                    output("", "none");
            }
            else
            {
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
            }

            if (not cat)
            {
                std::string ebuild;
                try
                {
                    ebuild = portage::ebuild_which(portdir, package);
                }
                catch (const portage::nonexistent_pkg_E)
                {
                    ebuild = portage::ebuild_which(real_portdir, package);
                }
                
                ebuild_vars.read(ebuild);

                if (quiet and ebuild_vars["HOMEPAGE"].empty())
                    ebuild_vars["HOMEPAGE"] = "none";

                if (not ebuild_vars["HOMEPAGE"].empty())
                {
                    std::vector<std::string> parts =
                        util::split(ebuild_vars["HOMEPAGE"]);

                    if (parts.size() >= 1)
                        output("Homepage", parts.front());

                    if (parts.size() > 1)
                    {
                        std::vector<std::string>::iterator h;
                        for (h = ( parts.begin() + 1) ; h != parts.end() ; ++h)
                            output("", *h);
                    }
                }
            }

            /* long description */
            if (longdesc.empty())
            {
                if (not cat)
                {
                    if (ebuild_vars["DESCRIPTION"].empty())
                        ebuild_vars["DESCRIPTION"] = "none";
                    
                    output("Description", ebuild_vars["DESCRIPTION"]);
                }
                else
                    output("Description", "none");
            }
            else
                output("Description", util::tidy_whitespace(longdesc));
        }

        /* package or category exists, but metadata.xml doesn't */
        else
        {
            if (quiet)
                output("", "No metadata.xml");
            else
                output("", color[red] + "No metadata.xml." + color[none]);
            
            /* at least show ebuild DESCRIPTION and HOMEPAGE */
            if (not cat)
            {
                std::string ebuild;
                try
                {
                    ebuild = portage::ebuild_which(portdir, package);
                }
                catch (const portage::nonexistent_pkg_E)
                {
                    ebuild = portage::ebuild_which(real_portdir, package);
                }
                
                util::vars_T ebuild_vars(ebuild);

                if (quiet and ebuild_vars["HOMEPAGE"].empty())
                    ebuild_vars["HOMEPAGE"] = "none";

                if (not ebuild_vars["HOMEPAGE"].empty())
                {
                    std::vector<std::string> parts =
                        util::split(ebuild_vars["HOMEPAGE"]);

                    if (parts.size() >= 1)
                        output("Homepage", parts.front());

                    if (parts.size() > 1)
                    {
                        std::vector<std::string>::iterator h;
                        for (h = ( parts.begin() + 1) ; h != parts.end() ; ++h)
                            output("", *h);
                    }
                }

                if (quiet and ebuild_vars["DESCRIPTION"].empty())
                    ebuild_vars["DESCRIPTION"] = "none";

                if (not ebuild_vars["DESCRIPTION"].empty())
                    output("Description", ebuild_vars["DESCRIPTION"]);
            }
        }
    }

    output.flush(*stream);

    /* give a numbered list of all the unique overlay's that were
     * encountered while processing the queries */
    if (portdirs.size() > 0)
    {
        *stream << std::endl << "Portage Overlays:" << std::endl;

        std::vector<std::string>::iterator p;
        n = 1;
        for (p = portdirs.begin() ; p != portdirs.end() ; ++p, ++n)
            *stream << "  " << color[cyan] << "[" << n << "]" << color[none]
                << " " << *p << std::endl;
    }

    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
