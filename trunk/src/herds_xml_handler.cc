/*
 * herdstat -- src/herds_xml_handler.cc
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

#include "common.hh"
#include "herds_xml_handler.hh"

void
HerdsXMLHandler_T::on_start_element(const Glib::ustring &name,
                                    const AttributeList &attr)
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
}

void
HerdsXMLHandler_T::on_end_element(const Glib::ustring &name)
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
}

void
HerdsXMLHandler_T::on_characters(const Glib::ustring &str)
{
    /* <herd><name> */
    if (in_herd_name)
    {
        cur_herd = str;
        herds[str] = new herd_T(str);
    }

    /* <herd><description> */
    else if (in_herd_desc)
        herds[cur_herd]->desc = str;

    /* <herd><email> */
    else if (in_herd_email)
    {
        /* append @gentoo.org if needed */
        herds[cur_herd]->mail =
            (str.find('@') == Glib::ustring::npos ? str + "@gentoo.org" : str);
    }

    /* <maintainer><email> */
    else if (in_maintainer_email)
    {
        cur_dev = util::lowercase(str);

        /* append @gentoo.org if needed */
        if (str.find('@') == Glib::ustring::npos)
            cur_dev.append("@gentoo.org");
        herds[cur_herd]->insert(std::make_pair(cur_dev, new dev_attrs_T()));
    }

    /* <maintainer><name> */
    else if (in_maintainer_name)
    {
        herd_T::iterator i = herds[cur_herd]->find(cur_dev);
        if (i != herds[cur_herd]->end())
            i->second->name = str;

        if (cur_dev == "flameeyes@gentoo.org")
        debug_msg("dev = '%s', maintainer name = '%s'", cur_dev.c_str(), str.c_str());
    }

    /* <maintainer><role> */
    else if (in_maintainer_role)
    {
        herd_T::iterator i = herds[cur_herd]->find(cur_dev);
        if (i != herds[cur_herd]->end())
            i->second->role = str;
    }
}

HerdsXMLHandler_T::~HerdsXMLHandler_T()
{
    for (herds_T::iterator i = herds.begin() ; i != herds.end() ; ++i)
    {
        herd_T::iterator h;
        for (h = i->second->begin() ; h != i->second->end() ; ++h)
            delete h->second;
        delete i->second;
    }
}

/* vim: set tw=80 sw=4 et : */
