/*
 * herdstat -- src/herds.cc
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

#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <cstdlib>

#include "devs.hh"
#include "util.hh"
#include "options.hh"
#include "formatter.hh"
#include "herds.hh"

/*
 * Display a herd and it's developers.
 */

void
herd_T::display(std::ostream &stream)
{
    options_T options;
    formatter_T out;
    util::color_map_T color;

    std::string user = util::current_user();

    /* sort devs */
    std::vector<std::string> sorted_devs;
    {
        for (iterator i = this->begin() ; i != this->end() ; ++i)
        {
            std::string dev = i->first;
            if (not options.verbose())
            {
                /* strip domain names from email addy */
                std::string::size_type pos = dev.find('@');
                if (pos != std::string::npos)
                    dev = dev.substr(0, pos);
                /* skip any invalid email addy's; for example, if dev 'joe'
                 * uses 'joe' in one place and then 'joe@gentoo.org' in
                 * another, it'll cause duplicates                          */
                else
                    continue;
            }
            sorted_devs.push_back(dev);
        }
    }
    std::sort(sorted_devs.begin(), sorted_devs.end());

    if (not options.quiet())
    {
        if (not name.empty())
            out.append("Herd", name);
        if (not mail.empty())
        {
            /* add @gentoo.org if its not there already */
            std::string::size_type pos = mail.find('@');
            if (pos == std::string::npos)
                mail += "@gentoo.org";
            out.append("Email", mail);
        }
        if (not desc.empty())
            out.append("Description", desc);
    }

    if (options.verbose())
    {
        out.append(util::sprintf("Developers(%d)", this->size()), "");

        std::vector<std::string>::iterator i;
        for (i = sorted_devs.begin() ; i != sorted_devs.end() ; ++i)
        {
            /* display full email; if the user is in the herd, hilite it
             * to make them feel speshul :) */
            if (*i == user)
                out.append("", color[yellow] + (*i) + color[none]);
            else
                out.append("", color[blue] + (*i) + color[none]);

            /* display developer attributes (name, role, etc) */
            iterator x;
            if ((x = this->find(*i)) != this->end())
                x->second->display(std::cout);
        }
    }
    else
    {
        if (options.quiet())
        {
            std::vector<std::string>::iterator i;
            for (i = sorted_devs.begin() ; i != sorted_devs.end() ; ++i)
                std::cout << *i << std::endl;
        }
        else
        {
            std::string::size_type oldlen = 0;

            /* if the current user is in the herd, we highlight the nick
             * and adjust maxctotal appropriately                       */
            std::vector<std::string>::iterator pos =
                std::find(sorted_devs.begin(), sorted_devs.end(), user);
            if (pos != sorted_devs.end())
            {
                *pos = color[yellow] + user + color[none];
    
                oldlen = out.maxctotal();
                out.set_maxctotal(oldlen + color[yellow].length() +
                    color[none].length());
            }

            out.append(util::sprintf("Developers(%d)", sorted_devs.size()), sorted_devs);

            /* ...and restore it */
            if (oldlen != 0)
                out.set_maxctotal(oldlen);
        }
    }
}

/*
 * Display all herds.
 */

void
herds_T::display(std::ostream &stream)
{
    options_T options;
    util::color_map_T color;
    formatter_T out;

    std::vector<std::string> hvec;
    float ndev = 0;
    std::string biggest_herdstr;
    herd_T::size_type biggest_herd = 0;

    iterator h;
    for (h = this->begin() ; h != this->end() ; ++h)
    {
        ndev += h->second->size();
        if (h->second->size() > biggest_herd)
        {
            biggest_herd = h->second->size();
            biggest_herdstr = h->first;
        }
    }

    if (options.verbose() and not options.quiet())
        out.append(util::sprintf("Herds(%d)", this->size()), "");

    /* for each herd in herds.xml... */
    size_type n = 0;
    for (h = this->begin() ; h != this->end() ; ++h)
    {
        if (options.quiet())
            stream << h->first << std::endl;
        else if (options.verbose())
        {
            /* herd name */
            out.append("", color[blue] + h->first + color[none]);

            /* description */
            iterator x;
            if ((x = this->find(h->first)) != this->end())
                out.append("", x->second->desc);

            if (++n != this->size())
                out.endl();
        }
        /* otherwise, store it for processing after the loop */
        else
            hvec.push_back(h->first);
    }

    if (hvec.size() > 0)
        out.append(util::sprintf("Herds(%d)", this->size()), hvec);
    
    if (not options.quiet())
    {
        if (options.verbose())
            out.endl();
        out.append("Avg devs/herd", util::sprintf("%.2f", ndev / this->size()));
        out.append(util::sprintf("Most devs(%d)", biggest_herd), biggest_herdstr);
    }
}

/* vim: set tw=80 sw=4 et : */
