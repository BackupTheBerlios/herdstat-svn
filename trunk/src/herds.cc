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
    std::string::size_type oldlen = 0;

    /* sort devs */
    std::vector<std::string> sorted_devs;
    {
        for (iterator i = this->begin() ; i != this->end() ; ++i)
        {
            std::string dev = i->first;
            if (not options.verbose())
            {
                /* strip domain names from email addr */
                std::string::size_type pos = dev.find('@');
                if (pos != std::string::npos)
                {
                    /* is the current user this user? */
                    if (dev == user and not options.quiet())
                    {
                        /* if so, highlight it ; we need to save maxctotal
                         * in order to modify it to compensate for color length */
                        oldlen = out.maxctotal();
                        dev = color[yellow] + dev.substr(0, pos) + color[none];
                    }
                    else
                        dev = dev.substr(0, pos);
                }
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
            out.append("Email", mail);
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
            /* if the current user is in the herd, we highlighted the nick
             * so adjust maxctotal appropriately                            */
            if (oldlen != 0)
                out.set_maxctotal(oldlen + color[yellow].length() +
                    color[none].length());

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

    iterator h;
    size_type n = 0;
    for (h = this->begin() ; h != this->end() ; ++h)
        ndev += h->second->size();

    if (options.verbose() and not options.quiet())
    {
        out.append("Avg devs/herd", util::sprintf("%.2f",
            ndev / this->size()));
        out.append(util::sprintf("Herds(%d)", this->size()), "");
    }

    /* for each herd in herds.xml... */
    for (h = this->begin() ; h != this->end() ; ++h)
    {
        if (options.quiet())
            stream << h->first << std::endl;
        else if (options.verbose())
        {
            out.append("", color[blue] + h->first + color[none]);

            iterator x;
            if ((x = this->find(h->first)) != this->end())
                out.append("", x->second->desc);

            if (++n != this->size())
                out.endl();
        }
        else
            hvec.push_back(h->first);
    }

    if (not options.verbose() and not options.quiet())
    {
        out.append(util::sprintf("Herds(%d)", this->size()), hvec);
        out.append("Avg devs/herd", util::sprintf("%.2f", ndev / this->size()));
    }
}

/* vim: set tw=80 sw=4 et : */
