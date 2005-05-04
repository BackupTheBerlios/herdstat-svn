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

#include <algorithm>

#include "common.hh"
#include "overlaydisplay.hh"
#include "formatter.hh"
#include "action_versions_handler.hh"

int
action_versions_handler_T::operator() (opts_type &opts)
{
    std::multimap<util::string, util::string> matches;
    const util::string real_portdir(config.portdir());
    util::string dir;
    bool pwd = false;
    OverlayDisplay_T od;

    output.set_maxlabel(8);
    output.set_maxdata(maxcol - output.maxlabel());
    output.set_quiet(quiet);
    output.set_attrs();

    if (all)
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
        util::string leftover;
        util::string path = util::getcwd();
        while (depth > 0)
        {
            util::string::size_type pos = path.rfind('/');
            if (pos != util::string::npos)
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
        dir = path;
        opts.push_back(leftover);
        
        debug_msg("set portdir to '%s'", dir.c_str());
        debug_msg("added '%s' to opts.", leftover.c_str());
    }
    else if (regex and opts.size() > 1)
    {
        std::cerr << "You may only specify one regular expression." << std::endl;
        return EXIT_FAILURE;
    }
    else if (regex)
    {
        util::regex_T::string_type re(opts.front());
        opts.clear();
        
        if (optget("eregex", bool))
            regexp.assign(re, REG_EXTENDED|REG_ICASE);
        else
            regexp.assign(re, REG_ICASE);
        
        matches = portage::find_package_regex(config, regexp, overlay);
        
        if (matches.empty())
        {
            std::cerr << "Failed to find any packages matching '" << re << "'."
                << std::endl;
            return EXIT_FAILURE;
        }

    }

    opts_type::iterator i;
    for (i = opts.begin() ; i != opts.end() ; ++i)
        matches.insert(std::make_pair("", *i));

    std::multimap<util::string, util::string>::iterator m;
    std::multimap<util::string, util::string>::size_type n = 1;
    for (m = matches.begin() ; m != matches.end() ; ++m, ++n)
    {
        util::string package;

        try
        {
            if (pwd)
                package = portage::find_package_in(dir, m->second);
            else if (regex and not m->first.empty())
            {
                dir = m->first;
                package = m->second;
            }
            else
            {
                std::pair<util::string, util::string> p =
                    portage::find_package(config, m->second, optget("overlay", bool));
                dir = p.first;
                package = p.second;
            }

            if (dir != portdir and not pwd)
                od.insert(dir);

            portage::versions_T versions(dir + "/" + package);

            /* versions would be empty if the directory exists, but no
             * ebuilds are there - in this case, use real PORTDIR. */
            if (versions.empty())
                versions.assign(portdir + "/" + package);

            if (not optget("quiet", bool))
            {
                if (dir == portdir or pwd)
                    output("Package", package);
                else
                    output("Package", package + od[dir]);

                output.endl();
            }

            portage::versions_T::iterator v;
            for (v = versions.begin() ; v != versions.end() ; ++v)
            {
                util::string s((*(*v))["PVR"]);
                util::string::size_type pos = s.rfind("-r0");
                if (pos != util::string::npos)
                    s = s.substr(0, pos);

                output("", s);
            }

            if (n != matches.size())
                output.endl();
        }
        catch (const portage::ambiguous_pkg_E &e)
        {
            std::cerr << e.name()
                << " is ambiguous. Possible matches are: "
                << std::endl << std::endl;

            std::vector<util::string>::const_iterator x;
            for (x = e.packages.begin() ; x != e.packages.end() ; ++x)
            {
                if (quiet or not optget("color", bool))
                    std::cerr << *x << std::endl;
                else
                    std::cerr << color[green] << *x << color[none] << std::endl;
            }

            if (matches.size() == 1)
                return EXIT_FAILURE;
        }
        catch (const portage::nonexistent_pkg_E &e)
        {
            std::cerr << m->second << " doesn't seem to exist." << std::endl;

            if (matches.size() == 1)
                return EXIT_FAILURE;
        }
    }

    flush();
    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
