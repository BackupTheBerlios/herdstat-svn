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

#include "action_meta_handler.hh"
#include "action_find_handler.hh"

int
action_find_handler_T::operator() (opts_type &opts)
{
    opts_type results;

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
                    regexp, overlay, &search_timer);
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

    std::multimap<util::string, util::string>::iterator m;
    for (m = matches.begin() ; m != matches.end() ; ++m)
    {
        std::pair<util::string, util::string> p;

        try
        {
            if (regex)
                p = *m;
            else
                p = portage::find_package(config, m->second,
                        overlay, &search_timer);
        }
        catch (const portage::ambiguous_pkg_E &e)
        {
            /* ambiguous still matches */
            std::copy(e.packages.begin(), e.packages.end(),
                std::back_inserter(results));

            continue;
        }
        catch (const portage::nonexistent_pkg_E &e)
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
                std::ostream_iterator<util::string>(*stream, "\n"));
        }
    }

    size = results.size();
    flush();
    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
