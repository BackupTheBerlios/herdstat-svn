/*
 * herdstat -- src/querycache_xml_handler.hh
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

#ifndef HAVE_QUERYCACHE_XML_HANDLER_HH
#define HAVE_QUERYCACHE_XML_HANDLER_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "xmlparser.hh"
#include "querycache.hh"

/*
 * Content Handler for our internal querycache.xml
 */

class querycacheXMLHandler_T : public XMLHandler_T
{
    public:
        typedef std::vector<pkgQuery_T> value_type;
        typedef value_type::iterator iterator;
        typedef value_type::size_type size_type;

        querycacheXMLHandler_T()
        {
            in_query = in_string = in_with = in_type =
            in_results = in_pkg = false;
        }

        virtual ~querycacheXMLHandler_T();

        value_type queries;

    protected:
        /* callbacks */
        virtual return_type
        START_ELEMENT(const string_type &, const attrs_type &);
        virtual return_type END_ELEMENT(const string_type &);
        virtual return_type CHARACTERS(const string_type &);

    private:
        /* internal state variables */
        bool in_query, in_string, in_with, in_type, in_results, in_pkg;
        util::string cur_pkg, cur_date;
};

#endif

/* vim: set tw=80 sw=4 et : */
