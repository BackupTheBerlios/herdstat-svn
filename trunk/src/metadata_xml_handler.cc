/*
 * herdstat -- src/metadata_xml_handler.cc
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

#include "util.hh"
#include "devs.hh"
#include "options.hh"
#include "metadata_xml_handler.hh"

bool
MetadataXMLHandler_T::start_element(const std::string &name,
                                    const attrs_type &attrs)
{
    if (name == "herd")
        in_herd = true;
    else if (name == "maintainer")
        in_maintainer = true;
    else if (name == "email" and in_maintainer)
        in_email = true;
    else if (name == "name" and in_maintainer)
        in_name = true;
    else if (name == "description")
        in_desc = true;
    else if (name == "longdescription")
    {
        std::string locale = optget("locale", std::string);
        attrs_type::const_iterator pos = attrs.find("lang");
        if (pos != attrs.end())
        {
            if (pos->second == locale.substr(0,2))
                in_longdesc = true;
            else if (locale == "C" and pos->second == "en")
                in_longdesc = true;
        }
        else
            in_longdesc = true;
    }

    return true;
}

bool
MetadataXMLHandler_T::end_element(const std::string &name)
{
    if (name == "herd")
        in_herd = false;
    else if (name == "maintainer")
        in_maintainer = false;
    else if (name == "email" and in_maintainer)
        in_email = false;
    else if (name == "name" and in_maintainer)
        in_name = false;
    else if (name == "description")
        in_desc = false;
    else if (name == "longdescription")
        in_longdesc = false;

    return true;
}

bool
MetadataXMLHandler_T::text(const std::string &str)
{
    /* <herd> */
    if (in_herd)
        herds.push_back(str);

    /* <maintainer><email> */
    else if (in_email)
    {
        cur_dev = (str.find('@') == std::string::npos ? str + "@gentoo.org" : str);
        devs[cur_dev] = new dev_attrs_T();
    }

    /* <maintainer><name> */
    else if (in_name)
        devs[cur_dev]->name = str;

    /* <maintainer><description> */
    else if (in_desc)
        devs[cur_dev]->role = str;

    /* <longdescription> */
    else if (in_longdesc)
        longdesc =str;

    return true;
}

MetadataXMLHandler_T::~MetadataXMLHandler_T()
{
    herd_T::iterator i;
    for (i = devs.begin() ; i != devs.end() ; ++i)
        delete i->second;
}

/* vim: set tw=80 sw=4 et : */
