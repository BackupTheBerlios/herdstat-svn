/*
 * herdstat -- src/action_away_handler.cc
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
#include "action_away_handler.hh"

int
action_away_handler_T::operator() (opts_type &opts)
{
    devaway_T::iterator d;
    devaway.fetch();
    devaway.parse();

    output.set_maxlabel(opts.empty() ? 16 : 12);
    output.set_maxdata(maxcol - output.maxlabel());
    output.set_attrs();

    if (opts.empty())
    {
        output(util::sprintf("Away Developers(%s)", devaway.size()), "");
        for (d = devaway.begin() ; d != devaway.end() ; ++d)
        {
            output("", color[blue] + d->first + color[none]);
            output("", util::tidy_whitespace(d->second));
            output.endl();
        }
    }
    else if (regex and opts.size() > 1)
    {
        std::cerr << "You may only specify one regular expression."
            << std::endl;
        return  EXIT_FAILURE;
    }
    else if (regex)
    {
        util::regex_T::string_type re(opts.front());
        opts.clear();

        regexp.assign(re, eregex ? REG_EXTENDED|REG_ICASE : REG_ICASE);

        for (d = devaway.begin() ; d != devaway.end() ; ++d)
            if (regexp == d->first)
                opts.push_back(d->first);

        if (opts.empty())
        {
            std::cerr << "Failed to find any developers matching '" << re
                << "'." << std::endl;
            return EXIT_FAILURE;
        }

        std::sort(opts.begin(), opts.end());
    }

    opts_type::iterator i;
    opts_type::size_type n = 1;
    for (i = opts.begin() ; i != opts.end() ; ++i)
    {

    }

    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
