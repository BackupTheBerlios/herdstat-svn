/*
 * herdstat -- src/action_find_handler.cc
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

#include <map>
#include <utility>
#include <algorithm>
#include <iterator>

#include <herdstat/portage/find.hh>
#include <herdstat/portage/exceptions.hh>

#include "pkgcache.hh"
#include "action_meta_handler.hh"
#include "action_find_handler.hh"

action_find_handler_T::action_find_handler_T()
    : action_portage_find_handler_T(), meta(optget("meta", bool))
{
}

action_find_handler_T::~action_find_handler_T()
{
}

int
action_find_handler_T::operator() (opts_type &opts)
{
    opts_type results;
    pkgcache_T pkgcache(portdir);

    if (all)
    {
        std::cerr << "find handler does not support the 'all' target."
            << std::endl;
        return EXIT_FAILURE;
    }
    else if (regex and opts.size() > 1)
    {
        std::cerr << "You may only specify one regular expression."
            << std::endl;
        return EXIT_FAILURE;
    }
    else if (regex)
    {
        regexp.assign(opts.front(), eregex ? REG_EXTENDED|REG_ICASE : 
                                             REG_ICASE);

        matches = portage::find_package_regex(config,
                    regexp, overlay, &search_timer, pkgcache.pkgs());
        if (matches.empty())
        {
            std::cerr << "Failed to find any packages matching '"
                << opts.front() << "'." << std::endl;
            return EXIT_FAILURE;
        }
    }
    else
    {
        opts_type::iterator i;
        for (i = opts.begin() ; i != opts.end() ; ++i)
            matches.insert(std::make_pair("", *i));
    }

    std::multimap<std::string, std::string>::iterator m;
    for (m = matches.begin() ; m != matches.end() ; ++m)
    {
        std::pair<std::string, std::string> p;

        try
        {
            if (regex)
                p = *m;
            else
                p = portage::find_package(config, m->second,
                        overlay, &search_timer, pkgcache.pkgs());
        }
        catch (const portage::AmbiguousPkg &e)
        {
            /* ambiguous still matches */
            results.insert(results.end(), e.packages.begin(), e.packages.end());
            continue;
        }
        catch (const portage::NonExistentPkg &e)
        {
            std::cerr << m->second << " doesn't seem to exist." << std::endl;

            if (matches.size() == 1)
                return EXIT_FAILURE;
            
            continue;
        }
        
        results.push_back(p.second);
    }

    if (results.size() > 1)
    {
        std::sort(results.begin(), results.end());
        results.erase(std::unique(results.begin(), results.end()),
            results.end());
    }

    if (meta)
    {
        /* disable stuff we've already handled */
        optset("regex", bool, false);
        optset("eregex", bool, false);
        
        action_meta_handler_T mhandler;
        mhandler(results);
    }
    else if (not count)
    {
        if (results.size() == 1)
            *stream << results.front() << std::endl;
        else
        {
            std::copy(results.begin(), results.end(), 
                std::ostream_iterator<std::string>(*stream, "\n"));
        }
    }

    size = results.size();
    flush();
    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
