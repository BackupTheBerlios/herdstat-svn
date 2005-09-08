/*
 * herdstat -- src/action_away_handler.cc
 * $Id: action_away_handler.cc 508 2005-09-03 11:30:08Z ka0ttic $
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
#include <herdstat/util/string.hh>

#include "action_away_handler.hh"

using namespace portage;

void
action_away_handler_T::display(Herd::iterator dev)
{
    if (quiet)
        *stream << dev->user() << " - " << dev->awaymsg() << std::endl;
    else
    {
        herdsxml.fill_developer(*dev);

        if (dev->name().empty())
            output("Developer", dev->user());
        else
            output("Developer", dev->name() + " (" + dev->user() + ")");

        output("Email", dev->email());
        output("Away Message", dev->awaymsg());
    }

    size += 1;
}

int
action_away_handler_T::operator() (opts_type &opts)
{
    devaway.fetch(optget("devaway.location", std::string));
    devaway.parse(optget("devaway.location", std::string));
    const Herd& devs(devaway.devs());
    Herd::const_iterator d;

    output.set_maxlabel(all ? 20 : 13);
    output.set_maxdata(maxcol - output.maxlabel());
    output.set_attrs();

    if (all)
    {
        output(util::sprintf("Away Developers(%d)", devs.size()), "");
        for (d = devs.begin() ; d != devs.end() ; ++d)
        {
            /* FIXME: use display() */

            if (quiet)
                *stream << d->user() << " - " << d->awaymsg() << std::endl;
            else
            {
                if (optget("color", bool))
                    output("", color[blue] + d->user() + color[none]);
                else
                    output("", d->user());

                output("", d->awaymsg());
                output.endl();
            }
        }

        size = devs.size();
        flush();
        return EXIT_SUCCESS;
    }
        
    herdsxml.fetch(optget("herds.xml", std::string));
    herdsxml.parse(optget("herds.xml", std::string));

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

        for (d = devs.begin() ; d != devs.end() ; ++d)
            if (regexp == d->user())
                opts.push_back(d->user());

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
            d = devs.find(*i);
            if (d == devs.end())
                throw DevException();

            display(d);
        }
        catch (const DevException)
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
