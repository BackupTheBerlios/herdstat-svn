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

#include <algorithm>

#include "common.hh"
#include "devs.hh"
#include "metadata_xml_handler.hh"

MetadataXMLHandler_T::return_type
MetadataXMLHandler_T::START_ELEMENT(const string_type &name,
                                    const attrs_type &attrs)
{
    if (name == "catmetadata")
        data.is_category = true;
    else if (name == "herd")
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
        util::string locale = optget("locale", util::string);

        attrs_type::const_iterator pos, pose = attrs.end();
        util::string value;
        for (pos = attrs.begin() ; pos != pose ; ++pos)
        {
#ifdef USE_LIBXMLPP
            if (pos->name == "lang")
                value = pos->value;
#else /* USE_LIBXMLPP */
            if (pos->first == "lang")
                value = pos->second;
#endif /* USE_LIBXMLPP */
        }

        if (not value.empty())
        {
            if (value == locale.substr(0,2))
                in_longdesc = true;
            else if ((locale == "C" or locale == "POSIX") and value == "en")
                in_longdesc = true;
        }
        else
            in_longdesc = true;
    }

#ifdef USE_XMLWRAPP
    return true;
#endif
}

MetadataXMLHandler_T::return_type
MetadataXMLHandler_T::END_ELEMENT(const string_type &name)
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

#ifdef USE_XMLWRAPP
    return true;
#endif
}

MetadataXMLHandler_T::return_type
MetadataXMLHandler_T::CHARACTERS(const string_type &str)
{
    /* <herd> */
    if (in_herd)
        data.herds.push_back(util::lowercase(str));

    /* <maintainer><email> */
    else if (in_email)
    {
        cur_dev = util::lowercase(str);

        if (str.find('@') == string_type::npos)
            cur_dev += "@gentoo.org";

        data.devs[cur_dev] = new dev_type();
    }

    /* <maintainer><name> */
    else if (in_name)
        data.devs[cur_dev]->name = str;

    /* <maintainer><description> */
    else if (in_desc)
        data.devs[cur_dev]->role = str;

    /* <longdescription> */
    else if (in_longdesc)
        data.longdesc = str;

#ifdef USE_XMLWRAPP
    return true;
#endif
}

/* vim: set tw=80 sw=4 et : */
