/*
 * herdstat -- src/action_dev_handler.cc
 * $Id: action_dev_handler.cc 508 2005-09-03 11:30:08Z ka0ttic $
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

#include <memory>
#include <herdstat/util/string.hh>

#include "common.hh"
#include "action_dev_handler.hh"

using namespace portage;

/*
 * Display data for the specified developer.
 */

void
action_dev_handler_T::display(const std::string &dev)
{
    Developer d(dev);
    herdsxml.fill_developer(d);
    const std::vector<std::string>& herds(d.herds());

    /* was the dev in any of the herds? */
    if (herds.empty())
        throw DevException();

    if (not quiet)
    {
        if (d.name().empty())
            output("Developer", dev);
        else
            output("Developer", d.name() + " (" + dev + ")");
        
        output("Email", d.email());
    }

    if (verbose and not quiet)
    {
        output(util::sprintf("Herds(%d)", herds.size()), "");

        std::vector<std::string>::const_iterator i;
        std::vector<std::string>::size_type nh = 1;
        for (i = herds.begin() ; i != herds.end() ; ++i, ++nh)
        {
            /* display herd */
            if (optget("color", bool))
                output("", color[blue] + (*i) + color[none]);
            else
                output("", *i);
                        
            /* display herd info */
            Herds::const_iterator h = herdsxml.herds().find(*i);
            if (not h->email().empty())
                output("", h->email());
            if (not h->desc().empty())
                output("", h->desc());

            if (nh != herds.size())
                output.endl();
        }
    }
    else if (not count)
        output(util::sprintf("Herds(%d)", herds.size()), herds);

    size += herds.size();
}

/*
 * Given a list of developers, display all herds that
 * each developer belongs to.
 */

int
action_dev_handler_T::operator() (opts_type &devs)
{
    herdsxml.fetch(optget("herds.xml", std::string));
    herdsxml.parse(optget("herds.xml", std::string));
    const Herds& herds(herdsxml.herds());
    Herds::const_iterator h;

    if (use_devaway)
    {
        devaway.fetch(optget("devaway.location", std::string));
        devaway.parse(optget("devaway.location", std::string));
    }

    /* set format attributes */
    output.set_maxlabel(all ? 16 : 12);
    output.set_maxdata(maxcol - output.maxlabel());
    if (use_devaway)
        output.set_devaway(devaway.keys());
    output.set_attrs();

    /* all target? */
    if (all)
    {
        /* treat ALL developers in herds.xml as a single herd */
        Herd all_devs;
        for (h = herds.begin() ; h != herds.end() ; ++h)
        {
            /* for each developer in the herd... */
            Herd::const_iterator d;
            for (d = h->begin() ; d != h->end() ; ++d)
            {
                /* if the developer is not already in our list, add it */
                if (all_devs.find(*d) == all_devs.end())
                    all_devs.push_back(*d);
            }
        }

        display_herd(all_devs, *stream);
        size = all_devs.size();
        flush();
        return EXIT_SUCCESS;
    }
    else if (regex and devs.size() > 1)
    {
        std::cerr << "You may only specify one regular expression."
            << std::endl;
        return EXIT_FAILURE;
    }
    else if (regex)
    {
        util::regex_T::string_type re(devs.front());
        devs.clear();

        regexp.assign(re, eregex ? REG_EXTENDED|REG_ICASE : REG_ICASE);

        /* loop through herds searching for devs who's username
         * matches the regular expression */
        for (h = herds.begin() ; h != herds.end() ; ++h)
        {
            Herd::const_iterator d = h->find(regexp);
            if (d != h->end())
                devs.push_back(d->user());
        }

        if (devs.empty())
        {
            std::cerr << "Failed to find any developers matching '" << re
                << "'." << std::endl;
            return EXIT_FAILURE;
        }

        std::sort(devs.begin(), devs.end());
        devs.erase(std::unique(devs.begin(), devs.end()), devs.end());
    }

    /* for each specified dev... */
    opts_type::iterator dev;
    opts_type::size_type n = 1;
    for (dev = devs.begin() ; dev != devs.end() ;  ++dev, ++n)
    {
        try
        {
            display(*dev);
        }
        catch (const DevException)
        {
            std::cerr << "Developer '" << *dev << "' doesn't seem to "
                << "belong to any herds." << std::endl;

            if (devs.size() == 1)
                return EXIT_FAILURE;
            else
                continue;
        }

        /* skip a line if we're not displaying the last one */
        if (not count and n != devs.size())
            output.endl();
    }

    flush();
    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
