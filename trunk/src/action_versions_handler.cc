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
#include <herdstat/util/misc.hh>
#include <herdstat/portage/exceptions.hh>
#include <herdstat/portage/find.hh>
#include <herdstat/portage/misc.hh>
#include <herdstat/portage/version.hh>

#include "pkgcache.hh"
#include "overlaydisplay.hh"
#include "formatter.hh"
#include "action_versions_handler.hh"

using namespace herdstat;
using namespace herdstat::util;

action_versions_handler_T::~action_versions_handler_T()
{
}

int
action_versions_handler_T::operator() (opts_type &opts)
{
    OverlayDisplay od;
    std::string dir;
    bool pwd = false;

    output.set_maxlabel(8);
    output.set_maxdata(options::maxcol() - output.maxlabel());
    output.set_quiet(options::quiet());
    output.set_attrs();

    if (not options::fields().empty())
    {
        std::cerr << "--field doesn't really make much sense with --versions."
            << std::endl;
        return EXIT_FAILURE;
    }

    if (options::all())
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

        /* Loop, trimming each directory from the end until depth == 0 */
        unsigned short depth = 2;
        std::string leftover;
        std::string path(util::getcwd());

        while (depth > 0)
        {
            std::string::size_type pos = path.rfind('/');
            if (pos != std::string::npos)
            {
                leftover = (leftover.empty() ? path.substr(pos + 1) :
                                               path.substr(pos + 1)+"/"+leftover);
                path.erase(pos);
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
    else if (options::regex())
    {
        regexp.assign(opts.front());
        opts.clear();
        
        pkgcache_T pkgcache(options::portdir());
        matches = portage::find_package_regex(regexp, options::overlay(),
                    &search_timer, pkgcache);
        
        if (matches.empty())
        {
            std::cerr << "Failed to find any packages matching '"
                << regexp() << "'." << std::endl;
            return EXIT_FAILURE;
        }
    }

    for (opts_type::iterator i = opts.begin() ; i != opts.end() ; ++i)
        matches.insert(std::make_pair("", *i));

    std::multimap<std::string, std::string>::iterator m;
    std::multimap<std::string, std::string>::size_type n = 1;
    for (m = matches.begin() ; m != matches.end() ; ++m, ++n)
    {
        std::string package;

        try
        {
            if (pwd)
                package = portage::find_package_in(dir,
                            m->second, &search_timer);
            else if (options::regex() and not m->first.empty())
            {
                dir = m->first;
                package = m->second;
            }
            else
            {
                std::pair<std::string, std::string> p =
                    portage::find_package(m->second, options::overlay(),
                        &search_timer);

                dir = p.first;
                package = p.second;
            }

            if (dir != options::portdir() and not pwd)
                od.insert(dir);

            portage::versions versions(dir + "/" + package);

            /* versions would be empty if the directory exists, but no
             * ebuilds are there - in this case, use real PORTDIR. */
            if (versions.empty())
                versions.assign(options::portdir() + "/" + package);
            if (versions.empty())
                throw portage::NonExistentPkg(package);

            size += versions.size();

            if (not options::quiet())
            {
                if (dir == options::portdir() or pwd)
                    output("Package", package);
                else
                    output("Package", package + od[dir]);

                output.endl();
            }

            if (not options::count())
            {
                portage::versions::iterator v;
                for (v = versions.begin() ; v != versions.end() ; ++v)
                {
                    const portage::version_map &vmap = v->components();

                    std::string s(vmap["PVR"]);
                    std::string::size_type pos = s.rfind("-r0");
                    if (pos != std::string::npos)
                        s.erase(pos);

                    output("", s);
                }
            }

            if (not options::count() and (n != matches.size()))
                output.endl();
        }
        catch (const portage::AmbiguousPkg &e)
        {
            std::cerr << e.name()
                << " is ambiguous. Possible matches are: "
                << std::endl << std::endl;

            opts_type::const_iterator i;
            for (i = e.packages.begin() ; i != e.packages.end() ; ++i)
            {
                if (options::quiet() or not options::color())
                    std::cerr << *i << std::endl;
                else
                    std::cerr << color[green] << *i << color[none] << std::endl;
            }

            if (matches.size() == 1)
                return EXIT_FAILURE;
        }
        catch (const portage::NonExistentPkg &e)
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
