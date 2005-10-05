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
#include <herdstat/util/string.hh>
#include <herdstat/util/algorithm.hh>

#include "exceptions.hh"
#include "action_away_handler.hh"

using namespace herdstat;
using namespace herdstat::portage;
using namespace herdstat::util;

action_away_handler_T::~action_away_handler_T()
{
}

void
action_away_handler_T::display(const Developer& dev)
{
    ++size;

    if (options::count())
        return;

    if (options::quiet())
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
        output("Away Message", util::tidy_whitespace(d.awaymsg()));
    }
}

int
action_away_handler_T::operator()(opts_type& opts)
{
    if (not options::fields().empty())
    {
        std::cerr << "--field doesn't really make much sense with --away."
            << std::endl;
        return EXIT_FAILURE;
    }

    output.set_maxlabel(13);
    output.set_maxdata(options::maxcol() - output.maxlabel());
    output.set_quiet(options::quiet(), " ");
    output.set_attrs();

    fetch_devawayxml();
    devaway.parse(options::devawayxml());
    fetch_herdsxml();
    herdsxml.parse(options::herdsxml());

    const Developers& devs(devaway.devs());
    Developers::const_iterator d;

    if (options::all())
    {
        for (d = devs.begin() ; d != devs.end() ; )
        {
            display(*d++);
            if (not options::quiet() and (d != devs.end()))
                output.endl();
        }

        flush();
        return EXIT_SUCCESS;
    }
    
    if (options::regex())
    {
        regexp.assign(opts.front());
        opts.clear();

        /* insert developer user names that match the regex into opts */
        transform_if(devs.begin(), devs.end(), std::back_inserter(opts),
            std::bind1st(UserRegexMatch<Developer>(), regexp), User());

        if (opts.empty())
        {
            std::cerr << "Failed to find any developers matching '"
                << regexp() << "'." << std::endl;
            return EXIT_FAILURE;
        }

        std::sort(opts.begin(), opts.end());
    }

    for (opts_type::iterator i = opts.begin() ; i != opts.end() ; ++i)
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

        if (not options::quiet() and ((i+1) != opts.end()))
            output.endl();
    }

    flush();
    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
