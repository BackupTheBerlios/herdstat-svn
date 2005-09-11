/*
 * herdstat -- src/action_dev_handler.cc
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

#include <memory>
#include <herdstat/util/string.hh>

#include "common.hh"
#include "action_herd_handler.hh"   /* for display_herd() */
#include "action_dev_handler.hh"

using namespace portage;

/* given string in form of "10 January 2000", get elapsed number of years.
 * returns empty string on any failure. */
static const std::string
get_elapsed_yrs(const std::string& joined)
{
    std::time_t now, joined_date;

    try
    {
        joined_date = util::str2epoch(joined.c_str(), "%d %b %Y");
        if (joined_date == static_cast<std::time_t>(-1))
            return std::string();
    }
    catch (const BadDate& e)
    {
        return std::string();
    }

    if ((now = std::time(NULL)) == static_cast<std::time_t>(-1))
        return std::string();

    double seconds = std::difftime(now, joined_date);
    return (util::sprintf("%.2f", seconds/31536000)+" yrs");
}

/*
 * Display data for the specified developer.
 */

void
action_dev_handler_T::display(const std::string &d)
{
    Developer dev(d);

    /* fill Developer object with data from each respective XML file */
    herdsxml.fill_developer(dev);
    devaway.fill_developer(dev);
    userinfo.fill_developer(dev);
    
    std::vector<std::string> herds(dev.herds());

    /* bail if userinfo.xml wasn't used and dev is not in any herds */
    if (userinfo.empty() and herds.empty())
        throw DevException();

    if (not quiet)
    {
        if (dev.name().empty())
            output("Developer", d);
        else
            output("Developer", dev.name() + " (" + d + ")");
        
        output("Email", dev.email());
    }

    if (herds.empty())
    {
        if (not count)
            output("Herds(0)", "none");
    }
    else
    {
        std::sort(herds.begin(), herds.end());

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

    if (not quiet)
    {
        if (not herds.empty() and verbose and not userinfo.empty())
            output.endl();

        if (not dev.pgpkey().empty())
            output("PGP Key ID", dev.pgpkey());
        if (not dev.joined().empty())
        {
            const std::string elapsed(get_elapsed_yrs(dev.joined()));
            if (elapsed.empty())
                output("Joined Date", dev.joined());
            else
                output("Joined Date", dev.joined() + " ("+elapsed+")");
        }
        if (not dev.birthday().empty())
        {
            const std::string elapsed(get_elapsed_yrs(dev.birthday()));
            if (elapsed.empty())
                output("Birth Date", dev.birthday());
            else
                output("Birth Date", dev.birthday() + " ("+elapsed+")");
        }
        if (not dev.status().empty())
            output("Status", dev.status());
        if (not dev.role().empty())
            output("Roles", dev.role());
        if (not dev.location().empty())
            output("Location", dev.location());
        if (dev.is_away() and not dev.awaymsg().empty())
            output("Devaway", dev.awaymsg());
    }
}

/*
 * Given a list of developers, display all herds that
 * each developer belongs to.
 */

int
action_dev_handler_T::operator() (opts_type &opts)
{
    /* herds.xml */
    if (herdsxml_path.empty())
        herdsxml.fetch();
    herdsxml.parse(herdsxml_path);

    const Herds& herds(herdsxml.herds());
    Herds::const_iterator h;

    /* devaway.xml */
    if (use_devaway)
    {
        if (devaway_path.empty())
            devaway.fetch();
        devaway.parse(devaway_path);
    }

    /* userinfo.xml */
    if (not userinfo_path.empty())
        userinfo.parse(userinfo_path);

    /* set format attributes */
    output.set_maxlabel(all ? 16 : 12);
    output.set_maxdata(maxcol - output.maxlabel());
    /* set away devs (for use in marking them when they occur in output) */
    if (use_devaway)
        output.set_devaway(devaway.keys());
    output.set_attrs();

    /* all target? */
    if (all)
    {
        Developers all_devs;
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

        /* insert those that exist in userinfo.xml but not herds.xml */
        if (not userinfo.empty())
        {
            const Developers& devs(userinfo.devs());
            Developers::const_iterator d;
            for (d = devs.begin() ; d != devs.end() ; ++d)
            {
                if (all_devs.find(*d) == all_devs.end())
                    all_devs.push_back(*d);
            }
        }

        Herd herd(all_devs);
        display_herd(herd);
        size = herd.size();
        flush();
        return EXIT_SUCCESS;
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

        regexp.assign(re, eregex ? REG_EXTENDED|REG_ICASE : REG_ICASE);

        /* loop through herds searching for devs who's username
         * matches the regular expression */
        for (h = herds.begin() ; h != herds.end() ; ++h)
        {
            Herd::const_iterator d;
            for (d = h->begin() ; d != h->end() ; ++d)
            {
                if (regexp == d->user())
                    opts.push_back(d->user());
            }
        }

        /* also add those in userinfo.xml - dupes will be unique'd out below */
        if (not userinfo.empty())
        {
            const Developers& devs(userinfo.devs());
            Developers::const_iterator d;
            for (d = devs.begin() ; d != devs.end() ; ++d)
            {
                if (regexp == d->user())
                    opts.push_back(d->user());
            }
        }

        if (opts.empty())
        {
            std::cerr << "Failed to find any developers matching '" << re
                << "'." << std::endl;
            return EXIT_FAILURE;
        }

        /* remove any dupes */
        std::sort(opts.begin(), opts.end());
        opts.erase(std::unique(opts.begin(), opts.end()), opts.end());
    }

    /* for each specified dev... */
    opts_type::iterator dev;
    opts_type::size_type n = 1;
    for (dev = opts.begin() ; dev != opts.end() ;  ++dev, ++n)
    {
        try
        {
            display(*dev);
        }
        catch (const DevException)
        {
            std::cerr << "Developer '" << *dev << "' doesn't seem to "
                << "belong to any herds." << std::endl;

            if (opts.size() == 1)
                return EXIT_FAILURE;
            else
                continue;
        }

        /* skip a line if we're not displaying the last one */
        if (not count and n != opts.size())
            output.endl();
    }

    flush();
    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
