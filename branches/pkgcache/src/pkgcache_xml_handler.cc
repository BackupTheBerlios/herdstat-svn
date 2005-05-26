/*
 * herdstat -- src/pkgcache_xml_handler.cc
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

#include "pkgcache_xml_handler.hh"

pkgCacheXMLHandler_T::return_type
pkgCacheXMLHandler_T::START_ELEMENT(const string_type &name,
                                    const attrs_type &attrs)
{
    if (name == "query")
    {
        attrs_type::const_iterator pos;

        for (pos = attrs.begin() ; pos != attrs.end() ; ++pos)
        {
#ifdef USE_LIBXMLPP
            if (pos->name == "id")
                cur_query_id = std::atoi(pos->value.c_str());
#else /* USE_LIBXMLPP */
            if (pos->first == "id")
                cur_query_id = std::atoi(pos->second.c_str());
#endif /* USE_LIBXMLPP */
        }

        in_query = true;
    }
    else if (name == "search")
        in_search = true;
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

pkgCacheXMLHandler_T::return_type
pkgCacheXMLHandler_T::END_ELEMENT(const string_type &name)
{
    if (name == "query")
        in_query = false;
    else if (name == "search")
        in_search = false;
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

pkgCacheXMLHandler_T::return_type
pkgCacheXMLHandler_T::CHARACTERS(const string_type &text)
{
    if (in_search)
    {
        cur_query.assign(text);
        pkgQuery_T *query = new pkgQuery_T(cur_query);
        query->id = cur_query_id;
        queries[cur_query] = query;
    }
    else if (in_date)
        queries[cur_query]->date = std::strtol(text.c_str(), NULL, 10);
    else if (in_pkgname)
        cur_pkg.assign(text);
    else if (in_pkglongdesc)
        (*queries[cur_query])[cur_pkg] = text;

#ifdef USE_XMLWRAPP
    return true;
#endif
}

pkgCacheXMLHandler_T::~pkgCacheXMLHandler_T()
{
    value_type::iterator i;
    for (i = queries.begin() ; i != queries.end() ; ++i)
        delete i->second;
}

/* vim: set tw=80 sw=4 et : */
