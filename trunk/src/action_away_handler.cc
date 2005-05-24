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

void
action_away_handler_T::display(const util::string &dev)
{
    herds_xml_T::devinfo_T info = herds_xml.get_dev_info(dev);

    if (not quiet)
    {
        if (info.name.empty())
            output("Developer", dev);
        else
            output("Developer", info.name + " (" + dev + ")");

        output("Email", dev + "@gentoo.org");
    }

    size += 1;
}

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

        size = devaway.size();
        flush();
        return EXIT_SUCCESS;
    }
        
    herds_xml.fetch();
    herds_xml.parse();

    if (regex and opts.size() > 1)
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
        try
        {
            display(*i);
        }
        catch (const dev_E)
        {
            std::cerr << "Developer '" << *i << "' either doesn't exist or "
                << "is not currently away." << std::endl;

            if (opts.size() == 1)
                return EXIT_FAILURE;
        }

        if (not count and n != opts.size())
            output.endl();
    }

    flush();
    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
