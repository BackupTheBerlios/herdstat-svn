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

std::pair<std::string, std::string>
search_overlays(const std::vector<std::string> &overlays,
                const std::string &pkg)
{
    std::pair<std::string, std::string> p;

    /* search overlays */
    std::vector<std::string>::const_iterator o;
    for (o = overlays.begin() ; o != overlays.end() ; ++o)
    {
        try
        {
            p.second = portage::find_package(*o, pkg);

            if (util::is_file(*o + "/" + p.second + "/metadata.xml"))
                p.first  = *o;
            else
                continue;
        }
        catch (const portage::nonexistent_pkg_E)
        {
            continue;
        }
    }

    return p;
}

int
action_meta_handler_T::operator() (std::vector<std::string> &opts)
{
    util::color_map_T color;
    formatter_T output;
    std::ostream *stream = optget("outstream", std::ostream *);
    const bool quiet = optget("quiet", bool);

    portage::config_T config(optget("portage.config", portage::config_T));
    std::string portdir(config.portdir());
    std::vector<std::string> overlays(config.overlays());

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

    /* check PORTDIR */
    if (not util::is_dir(portdir))
        throw bad_fileobject_E(portdir);

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
        std::string longdesc, metadata, package;

        std::pair<std::string, std::string> p;
        try
        {
            /* search PORTDIR (or $PWD if opts == 0) */
            package = portage::find_package(portdir, *i);

            p = search_overlays(overlays, *i);

            /* found in a overlay? */
            if (not p.second.empty())
            {
                /* is it a package? or a category whose metadata exists? */
                if ((p.second.find('/') != std::string::npos) or
                    util::is_file(p.first + "/" + p.second + "/metadata.xml"))
                {
                    portdir = p.first;
                    package = p.second;
                }
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
            bool found = false;

            p = search_overlays(overlays, *i);

            /* found in a overlay? */
            if (not p.second.empty())
            {
                /* is it a package? or a category whose metadata exists? */
                if ((p.second.find('/') != std::string::npos) or
                    util::is_file(p.first + "/" + p.second + "/metadata.xml"))
                {
                    found = true;
                    portdir = p.first;
                    package = p.second;
                }
            }

            if (not found)
            {
                std::cerr << *i << " doesn't seem to exist." << std::endl;

                if (opts.size() == 1)
                    return EXIT_FAILURE;
                else
                    continue;
            }
        }

        /* if no '/' exists, assume it's a category */
        cat = (package.find('/') == std::string::npos);

        if (n != 1)
            output.endl();

        output(cat ? "Category" : "Package", package);

        if (util::is_file(portdir + "/" + package + "/metadata.xml"))
        {
            util::vars_T ebuild_vars;

            /* parse it */
            try
            {
                std::auto_ptr<MetadataXMLHandler_T>
                    handler(new MetadataXMLHandler_T());
                XMLParser_T parser(&(*handler));

                parser.parse(portdir + "/" + package + "/metadata.xml");

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
                std::string ebuild(portage::ebuild_which(package));
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
                util::vars_T ebuild_vars(portage::ebuild_which(package));
                
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
    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
