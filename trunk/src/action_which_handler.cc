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
action_which_handler_T::operator() (std::vector<std::string> &opts)
{
    util::color_map_T color;
    std::ostream *stream = optget("outstream", std::ostream *);
    portage::config_T config(optget("portage.config", portage::config_T));
    const std::string real_portdir(config.portdir());
    const bool regex = optget("regex", bool);

    if (optget("all", bool))
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
        util::regex_T regexp;
        std::string re(opts.front());
        opts.clear();

        if (optget("eregex", bool))
            regexp.assign(re, REG_EXTENDED|REG_ICASE);
        else
            regexp.assign(re, REG_ICASE);

        const std::multimap<std::string, std::string> matches =
            portage::find_package_regex(config, regexp, optget("overlay", bool));
        std::multimap<std::string, std::string>::const_iterator m;
        for (m = matches.begin() ; m != matches.end() ; ++m)
        {
            if (std::find(opts.begin(), opts.end(), m->second) == opts.end())
                opts.push_back(m->second);
        }

        if (matches.empty())
        {
            std::cerr << "Failed to find any packages matching '" << re << "'."
                << std::endl;
            return EXIT_FAILURE;
        }
    }

    std::vector<std::string>::iterator i;
    for (i = opts.begin() ; i != opts.end() ; ++i)
    {
        std::string ebuild;
        std::pair<std::string, std::string> p;

        try
        {
            p = portage::find_package(config, *i, optget("overlay", bool));
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

        try
        {
            ebuild = portage::ebuild_which(p.first, p.second);
        }
        catch (const portage::nonexistent_pkg_E)
        {
            ebuild = portage::ebuild_which(real_portdir, p.second);
        }

        *stream << ebuild << std::endl;
    }

    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
