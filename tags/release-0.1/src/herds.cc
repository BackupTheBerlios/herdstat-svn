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

#include <utility>
#include "types.hh"
#include "herds.hh"

bool
HerdsXMLHandler_T::start_element(const std::string &name, const attrs_type &)
{
    if (name == "herd")
        in_herd = true;
    else if (name == "name" and not in_maintainer)
        in_herd_name = true;
    else if (name == "email" and not in_maintainer)
        in_herd_email = true;
    else if (name == "description" and not in_maintainer)
        in_herd_desc = true;
    else if (name == "maintainer")
        in_maintainer = true;
    else if (name == "email" and in_maintainer)
        in_maintainer_email = true;
    else if (name == "name" and in_maintainer)
        in_maintainer_name = true;
    else if (name == "role")
        in_maintainer_role = true;
    return true;
}

bool
HerdsXMLHandler_T::end_element(const std::string &name)
{
    if (name == "herd")
        in_herd = false;
    else if (name == "name" and not in_maintainer)
        in_herd_name = false;
    else if (name == "email" and not in_maintainer)
        in_herd_email = false;
    else if (name == "description" and not in_maintainer)
        in_herd_desc = false;
    else if (name == "maintainer")
        in_maintainer = false;
    else if (name == "email" and in_maintainer)
        in_maintainer_email = false;
    else if (name == "name" and in_maintainer)
        in_maintainer_name = false;
    else if (name == "role")
        in_maintainer_role = false;
    return true;
}

bool
HerdsXMLHandler_T::text(const std::string &str)
{
    if (in_herd_name)
    {
        cur_herd = str;
        herds.insert(std::make_pair(str, new devs_T()));
    }
    else if (in_herd_desc)
        descs.insert(std::make_pair(cur_herd, str));
    else if (in_maintainer_email)
    {
        cur_dev = str;
        herds[cur_herd]->insert(std::make_pair(str, new dev_attrs_T()));
    }
    else if (in_maintainer_name)
    {
        devs_T::iterator i = herds[cur_herd]->find(cur_dev);
        if (i == herds[cur_herd]->end())
            return false;
        (i->second)->push_back(str);
    }
    else if (in_maintainer_role)
    {
        devs_T::iterator i = herds[cur_herd]->find(cur_dev);
        if (i == herds[cur_herd]->end())
            return false;
        (i->second)->push_back(str);
    }

    return true;
}

HerdsXMLHandler_T::~HerdsXMLHandler_T()
{
    herds_T::iterator i;
    for (i = herds.begin() ; i != herds.end() ; ++i)
    {
        devs_T::iterator d;
        for (d = i->second->begin() ; d != i->second->end() ; ++d)
            delete d->second;

        delete i->second;
    }
}

/* vim: set tw=80 sw=4 et : */
