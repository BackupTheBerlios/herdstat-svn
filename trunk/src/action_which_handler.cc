/*
 * herdstat -- src/action_which_handler.cc
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
#include <herdstat/portage/exceptions.hh>
#include <herdstat/portage/find.hh>
#include "pkgcache.hh"
#include "action_which_handler.hh"

using namespace util;

action_which_handler_T::~action_which_handler_T()
{
}

int
action_which_handler_T::operator() (opts_type &opts)
{
    if (not options::fields().empty())
    {
        std::cerr << "--field doesn't really make much sense with --away."
            << std::endl;
        return EXIT_FAILURE;
    }

    if (options::all())
    {
        std::cerr << "which action handler does not support the 'all' target."
            << std::endl;
        return EXIT_FAILURE;
    }

    pkgcache_T pkgcache;
    std::multimap<std::string, std::string>::iterator m;

    if (options::regex())
    {
        regexp.assign(opts.front());
        opts.clear();

        pkgcache.init();
        matches = portage::find_package_regex(regexp, options::overlay(),
                    &search_timer, pkgcache);

        if (matches.empty())
        {
            std::cerr << "Failed to find any packages matching '"
                << regexp() << "'." << std::endl;
            return EXIT_FAILURE;
        }

        for (m = matches.begin() ; m != matches.end() ; ++m)
        {
            if (std::find(opts.begin(), opts.end(), m->second) == opts.end())
                opts.push_back(m->second);
            else
                matches.erase(m--);
        }
    }
    else
    {
        for (opts_type::iterator i = opts.begin() ; i != opts.end() ; ++i)
            matches.insert(std::make_pair("", *i));
    }

    for (m = matches.begin() ; m != matches.end() ; ++m)
    {
        std::string ebuild;
        std::pair<std::string, std::string> p;

        try
        {
            if (options::regex())
                p = *m;
            else
                p = portage::find_package(m->second, options::overlay(),
                        &search_timer);
        }
        catch (const portage::AmbiguousPkg &e)
        {
            std::cerr << e.name()
                << " is ambiguous. Possible matches are: "
                << std::endl << std::endl;
            
            std::vector<std::string>::const_iterator i;
            for (i = e.packages.begin() ; i != e.packages.end() ; ++i)
            {
                if (options::quiet() or not options::color())
                    std::cerr << *i << std::endl;
                else
                    std::cerr << color[green] << *i << color[none] << std::endl;
            }

            if (matches.size() == 1)
                return EXIT_FAILURE;

            continue;
        }
        catch (const portage::NonExistentPkg &e)
        {
            std::cerr << m->second << " doesn't seem to exist." << std::endl;

            if (matches.size() == 1)
                return EXIT_FAILURE;

            continue;
        }

        if (options::regex())
            ebuild = portage::ebuild_which(p.second, options::overlay(), NULL,
                pkgcache);
        else
            ebuild = portage::ebuild_which(p.second, options::overlay(), NULL);

        if (not options::count())
            *stream << ebuild << std::endl;
    }

    size = matches.size();

    flush();
    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
