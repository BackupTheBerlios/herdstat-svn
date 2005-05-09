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

#include "common.hh"
#include "action_which_handler.hh"

int
action_which_handler_T::operator() (opts_type &opts)
{
    std::multimap<util::string, util::string> matches;

    if (all)
    {
        std::cerr << "which action handler does not support the 'all' target."
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
        util::regex_T::string_type re(opts.front());
        opts.clear();

        if (eregex)
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
        util::string ebuild;
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
            std::cerr << std::endl << e.name()
                << " is ambiguous. Possible matches are: "
                << std::endl << std::endl;
            
            std::vector<util::string>::const_iterator i;
            for (i = e.packages.begin() ; i != e.packages.end() ; ++i)
            {
                if (optget("quiet", bool) or not optget("color", bool))
                    std::cerr << *i << std::endl;
                else
                    std::cerr << color[green] << *i << color[none] << std::endl;
            }

            if (matches.size() == 1)
                return EXIT_FAILURE;

            continue;
        }
        catch (const portage::nonexistent_pkg_E &e)
        {
            std::cerr << m->second << " doesn't seem to exist." << std::endl;

            if (matches.size() == 1)
                return EXIT_FAILURE;

            continue;
        }

        try
        {
            /* try p.first (may be an overlay) first */
            ebuild = portage::ebuild_which(p.first, p.second);
        }
        catch (const portage::nonexistent_pkg_E)
        {
            /* nope, so use real PORTDIR */
            ebuild = portage::ebuild_which(portdir, p.second);
        }

        if (not count)
            *stream << ebuild << std::endl;
    }

    size = matches.size();

    flush();
    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
