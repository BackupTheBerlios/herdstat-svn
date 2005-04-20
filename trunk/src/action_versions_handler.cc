/*
 * herdstat -- src/action_versions_handler.cc
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

#include "common.hh"
#include "formatter.hh"
#include "action_versions_handler.hh"

int
action_versions_handler_T::operator() (std::vector<std::string> &opts)
{
    util::color_map_T color;
    std::ostream *stream = optget("outstream", std::ostream *);
    portage::config_T config(optget("portage.config", portage::config_T));
    const std::string real_portdir(config.portdir());
    std::string portdir;
    bool pwd = false;

    formatter_T output;
    output.set_maxlabel(8);
    output.set_maxdata(optget("maxcol", std::size_t) - output.maxlabel());
    output.set_quiet(optget("quiet", bool));
    output.set_attrs();

    if (optget("all", bool))
    {
        std::cerr << "Versions action handler does not support the 'all' target."
            << std::endl;
        return EXIT_FAILURE;
    }

    /* we can be called with 0 opts if we are currently
     * in a package directory */
    if (opts.empty())
    {
        /* are we in a package's directory? */
        if (not portage::in_pkg_dir())
        {
            std::cerr << "You must be in a package directory if you want to run" << std::endl
                << PACKAGE << " --versions with no non-option arguments." << std::endl;
            return EXIT_FAILURE;
        }

        unsigned short depth = 2;

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

    std::vector<std::string>::iterator i;
    std::vector<std::string>::size_type n = 1;
    for (i = opts.begin() ; i != opts.end() ; ++i, ++n)
    {
        std::string package;

        try
        {
            if (pwd)
                package = portage::find_package_in(portdir, *i);
            else
            {
                std::pair<std::string, std::string> p =
                    portage::find_package(config, *i, optget("overlay", bool));
                portdir = p.first;
                package = p.second;
            }

            portage::versions_T versions(portdir + "/" + package);

            /* versions would be empty if the directory exists, but no
             * ebuilds are there - in this case, use real PORTDIR. */
            if (versions.empty())
                versions.assign(real_portdir + "/" + package);

            if (not optget("quiet", bool))
            {
                output("Package", package);
                output.endl();
            }

            portage::versions_T::iterator v;
            for (v = versions.begin() ; v != versions.end() ; ++v)
            {
                std::string s((*(*v))["PVR"]);
                std::string::size_type pos = s.rfind("-r0");
                if (pos != std::string::npos)
                    s = s.substr(0, pos);

                output("", s);
            }

            if (n != opts.size())
                output.endl();
        }
        catch (const portage::ambiguous_pkg_E &e)
        {
            std::cerr << e.name()
                << " is ambiguous. Possible matches are: "
                << std::endl << std::endl;

            std::vector<std::string>::const_iterator i;
            for (i = e.packages.begin() ; i != e.packages.end() ; ++i)
            {
                if (optget("quiet", bool) or not optget("color", bool))
                    std::cerr << *i << std::endl;
                else
                    std::cerr << color[green] << *i << color[none] << std::endl;
            }

            if (opts.size() == 1)
                return EXIT_FAILURE;
        }
        catch (const portage::nonexistent_pkg_E &e)
        {
            std::cerr << *i << " doesn't seem to exist." << std::endl;

            if (opts.size() == 1)
                return EXIT_FAILURE;
        }
    }

    output.flush(*stream);

    return EXIT_SUCCESS;
};

/* vim: set tw=80 sw=4 et : */
