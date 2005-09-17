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

#include <iostream>
#include <algorithm>
#include <herdstat/util/string.hh>
#include <herdstat/util/algorithm.hh>

#include "action_away_handler.hh"

using namespace portage;

action_away_handler_T::~action_away_handler_T()
{
}

void
action_away_handler_T::display(const Developer& dev)
{
    ++size;

    if (count) return;

    if (quiet)
        output("", dev.user() + " - " + dev.awaymsg());
    else
    {
        Developer d(dev);
        herdsxml.fill_developer(d);

        if (d.name().empty())
            output("Developer", d.user());
        else
            output("Developer", d.name() + " (" + d.user() + ")");

        output("Email", d.email());
        output("Away Message", d.awaymsg());
    }
}

int
action_away_handler_T::operator() (opts_type &opts)
{
    fetch_devawayxml();
    devaway.parse(devaway_path);
    const Developers& devs(devaway.devs());
    Developers::const_iterator d;

    output.set_maxlabel(13);
    output.set_maxdata(maxcol - output.maxlabel());
    output.set_quiet(quiet, " ");
    output.set_attrs();

    if (all)
    {
        for (d = devs.begin() ; d != devs.end() ; )
        {
            display(*d++);

            if (not quiet and (d != devs.end()))
                output.endl();
        }

        flush();
        return EXIT_SUCCESS;
    }
    
    fetch_herdsxml();
    herdsxml.parse(herdsxml_path);

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

        /* insert user names that match the regex into opts */
        util::transform_if(devs.begin(), devs.end(), std::back_inserter(opts),
            std::bind1st(UserRegexMatch<Developer>(), &regexp), User());

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
    for (i = opts.begin() ; i != opts.end() ; ++i, ++n)
    {
        try
        {
            if ((d = devs.find(*i)) == devs.end())
                throw DevException();

            display(*d);
        }
        catch (const DevException)
        {
            std::cerr << "Developer '" << *i << "' either doesn't exist or "
                << "is not currently away." << std::endl;

            if (opts.size() == 1)
                return EXIT_FAILURE;
        }

        if (not quiet and n != opts.size())
            output.endl();
    }

    flush();
    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
