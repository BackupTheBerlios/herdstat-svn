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
 * Return a vector of all keys
 */

std::vector<std::string>
herd_T::keys()
{
    std::vector<std::string> v;
    for (iterator i = this->begin() ; i != this->end() ; ++i)
        v.push_back(i->first);
    return v;
}

/*
 * Display a herd and it's developers.
 */

void
herd_T::display(std::ostream &stream)
{
    formatter_T out;
    util::color_map_T color;
    std::string user = util::current_user();
    std::string::size_type oldlen = 0;
    std::vector<std::string> devs = this->keys();
    std::vector<std::string>::iterator i;

    /* display header */
    if (not optget("quiet", bool))
    {
        if (not name.empty())
            out.append("Herd", name);

        if (not mail.empty())
            out.append("Email", mail);

        if (not desc.empty())
        {
            out.append("Description", desc);
            util::debug_msg("Description(%s): '%s'", name.c_str(), desc.c_str());
        }

        if (optget("verbose", bool))
            out.append(util::sprintf("Developers(%d)", this->size()), "");
    }
    
    for (i = devs.begin() ; i != devs.end() ; ++i)
    {
        if (optget("verbose", bool))
        {
            /* highlight email if current user is in the herd */
            if (*i == user)
                out.append("", color[yellow] + (*i) + color[none]);
            else
                out.append("", color[blue] + (*i) + color[none]);

            /* display developer attributes (name, role, etc) */
            iterator x;
            if ((x = this->find(*i)) != this->end())
                x->second->display(stream);
        }
        else
        {
            *i = util::get_user_from_email(*i);

            /* if the current user is in the herd, we highlight the nick
             * and adjust maxctotal appropriately */

            if (not optget("quiet", bool) and (user == (*i + "@gentoo.org")))
            {
                *i = color[yellow] + (*i) + color[none];

                oldlen = out.maxctotal();
                out.set_maxctotal(oldlen + color[yellow].length() +
                    color[none].length());
            }
        }
    }

    if (not optget("verbose", bool))
        out.append(util::sprintf("Developers(%d)", devs.size()), devs);

    if (oldlen != 0)
        out.set_maxctotal(oldlen);
}

/*
 * Display all herds.
 */

void
herds_T::display(std::ostream &stream)
{
    util::color_map_T color;
    formatter_T out;

    std::vector<std::string> hvec;

    if (optget("verbose", bool) and not optget("quiet", bool))
        out.append(util::sprintf("Herds(%d)", this->size()), "");

    /* for each herd in herds.xml... */
    size_type n = 0;
    for (iterator h = this->begin() ; h != this->end() ; ++h)
    {
        if (optget("verbose", bool))
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
}

/* vim: set tw=80 sw=4 et : */
