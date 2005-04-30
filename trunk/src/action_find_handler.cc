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
#include "action_find_handler.hh"

int
action_find_handler_T::operator() (opts_type &opts)
{
    std::ostream *stream = optget("outstream", std::ostream *);
    portage::config_T config(optget("portage.config", portage::config_T));
    const bool regex = optget("regex", bool);
    const bool overlay = optget("overlay", bool);
    std::multimap<util::string, util::string> matches;
    std::vector<util::string> results;

    if (optget("all", bool))
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
        util::regex_T regexp;
        util::regex_T::string_type re(opts.front());

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
                p = portage::find_package(config, m->second, overlay);
        }
        catch (const portage::ambiguous_pkg_E &e)
        {
            std::vector<util::string>::const_iterator i;
            for (i = e.packages.begin() ; i != e.packages.end() ; ++i)
                *stream << *i << std::endl;

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

    std::sort(results.begin(), results.end());
    results.erase(std::unique(results.begin(), results.end()), results.end());

    if (optget("count", bool))
        *stream << results.size() << std::endl;
    else
    {
        std::copy(results.begin(), results.end(), 
            std::ostream_iterator<util::string>(*stream, "\n"));
    }

    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
