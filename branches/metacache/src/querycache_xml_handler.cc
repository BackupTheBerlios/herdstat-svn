/*
 * herdstat -- src/querycache_xml_handler.cc
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

#include "querycache_xml_handler.hh"

querycacheXMLHandler_T::return_type
querycacheXMLHandler_T::START_ELEMENT(const string_type &name,
                                    const attrs_type &attrs)
{
    if (name == "query")
        in_query = true;
    else if (name == "string")
        in_string = true;
    else if (name == "with")
        in_with = true;
    else if (name == "type")
        in_type = true;
    else if (name == "date")
        in_date = true;
    else if (name == "results")
        in_results = true;
    else if (name == "pkg" and in_results)
        in_pkg = true;
    else if (name == "name" and in_pkg)
        in_pkgname = true;
    else if (name == "longdesc" and in_pkg)
        in_pkglongdesc = true;

#ifdef USE_XMLWRAPP
    return true;
#endif
}

querycacheXMLHandler_T::return_type
querycacheXMLHandler_T::END_ELEMENT(const string_type &name)
{
    if (name == "query")
        in_query = false;
    else if (name == "string")
        in_string = false;
    else if (name == "with")
        in_with = false;
    else if (name == "type")
        in_type = false;
    else if (name == "date")
        in_date = false;
    else if (name == "results")
        in_results = false;
    else if (name == "pkg")
        in_pkg = false;
    else if (name == "name")
        in_pkgname = false;
    else if (name == "longdesc")
        in_pkglongdesc = false;

#ifdef USE_XMLWRAPP
    return true;
#endif
}

querycacheXMLHandler_T::return_type
querycacheXMLHandler_T::CHARACTERS(const string_type &text)
{
    if (in_string)
        queries.push_back(pkgQuery_T(text));
    else if (in_with)
        queries.back().with = text;
    else if (in_type)
        queries.back().type =
            (text == "dev" ? QUERYTYPE_DEV : QUERYTYPE_HERD);
    else if (in_date)
        queries.back().date = std::strtol(text.c_str(), NULL, 10);
    else if (in_pkgname)
    {
        cur_pkg.assign(text);
        (queries.back())[cur_pkg] = "";
    }
    else if (in_pkglongdesc)
        (queries.back())[cur_pkg] = text;

#ifdef USE_XMLWRAPP
    return true;
#endif
}

querycacheXMLHandler_T::~querycacheXMLHandler_T()
{
//    value_type::iterator i;
//    for (i = queries.begin() ; i != queries.end() ; ++i)
//        delete *i;
}

/* vim: set tw=80 sw=4 et : */
