/*
 * herdstat -- src/action_bday_handler.cc
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
#include <functional>
#include <algorithm>

#include <herdstat/util/algorithm.hh>
#include <herdstat/util/regex.hh>
#include <herdstat/util/misc.hh>
#include "action_bday_handler.hh"

using namespace portage;

/* is the developer's birthday in this month? */
struct IsBdayDev : std::binary_function<Developer, const util::Regex *, bool>
{
    bool operator()(const Developer& dev, const util::Regex *monthre) const
    { return (*monthre == dev.birthday()); }
};

/* display the birthday dev */
static void
display(const Developer& dev)
{
    if (options::count())
        return;

    formatter_T out;

    if (options::quiet())
        out("Developer", dev.user());
    else
    {
        if (dev.name().empty())
            out("Developer", dev.user());
        else
            out("Developer", dev.name() + " (" + dev.user() + ")");

        out("Email", dev.email());

        const std::string elapsed(util::get_elapsed_yrs(dev.birthday()));
        if (elapsed.empty())
            out("Birth Date", dev.birthday());
        else
            out("Birth Date", dev.birthday() + " (" + elapsed + ")");

        out.endl();
    }
}

action_bday_handler_T::action_bday_handler_T()
{
}

action_bday_handler_T::~action_bday_handler_T()
{
}

int
action_bday_handler_T::operator()(opts_type &opts)
{
    if (options::userinfoxml().empty())
    {
        std::cerr << "The birthday action handler requires that you set the location" << std::endl
                  << "to userinfo.xml.  This can be done explicitly with the -U flag" << std::endl
                  << "or the 'userinfo' config option or implicitly with the -C flag" << std::endl
                  << "or the 'gentoo.cvs' config option." << std::endl;
        return EXIT_FAILURE;
    }

    userinfo.parse(options::userinfoxml());

    if (options::devaway())
    {
        fetch_devawayxml();
        devaway.parse(options::devawayxml());
    }

    output.set_maxlabel(12);
    output.set_maxdata(options::maxcol() - output.maxlabel());
    if (options::devaway())
        output.set_devaway(devaway.keys());
    output.set_attrs();

    /* if no opts were specified, use current month */
    const util::Regex monthre(opts.empty() ?
        util::format_date(std::time(NULL), "%B") : opts.front());

    Developers bday_devs;
    
    /* copy the birthday devs */
    util::copy_if(userinfo.devs().begin(), userinfo.devs().end(),
        std::inserter(bday_devs, bday_devs.end()),
        std::bind2nd(IsBdayDev(), &monthre));

    if (bday_devs.empty())
    {
        *stream << "There doesn't seem to be any developers" << std::endl
                  << "with a birthday in " << monthre() << "." << std::endl;
        return EXIT_SUCCESS;
    }

    std::for_each(bday_devs.begin(), bday_devs.end(), display);
    size = bday_devs.size();

    flush();
    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
