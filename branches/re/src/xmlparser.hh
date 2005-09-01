/*
 * herdstat -- src/xmlparser.hh
 * $Id$
 * Copyright (c) 2005 Aaron Walker <ka0ttic at gentoo.org>
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

#ifndef HAVE_PARSER_HH
#define HAVE_PARSER_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <istream>
//#include <xmlwrapp/init.h>
//#include <xmlwrapp/event_parser.h>
#include "saxparser.hh"

/*
 * Abstract XML Content Handler
 */

class XMLHandler_T : public SAXParser
{
    protected:
        virtual void start_element(const std::string &, const attrs_type &) = 0;
        virtual void end_element(const std::string &) = 0;
        virtual void characters(const std::string &) = 0;
};

/*
 * XML Parser - takes a pointer to a XMLHandler_T object
 */

class XMLParser_T
{
    public:
        typedef XMLHandler_T value_type;
        typedef std::string string_type;

        explicit XMLParser_T(value_type *h, bool validate = false) : handler(h)
        {
//            _init.remove_whitespace(true);
//            _init.validate_xml(validate);
        }

        virtual ~XMLParser_T() { }

        /* parse the given filename */
        virtual void parse(const string_type &path)
        {
            handler->parse_file(path);
        }

    protected:
        value_type *handler;

    private:
//        static xml::init _init;
};

#endif

/* vim: set tw=80 sw=4 et : */
