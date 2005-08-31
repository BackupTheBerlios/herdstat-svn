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
#include <xmlwrapp/init.h>
#include <xmlwrapp/event_parser.h>
#include "common.hh"

/*
 * Abstract XML Content Handler
 */

class XMLHandler_T : public xml::event_parser
{
    protected:
        virtual bool start_element(const std::string &, const attrs_type &) = 0;
        virtual bool end_element(const std::string &) = 0;
        virtual bool text(const std::string &) = 0;
};

/*
 * parser exception class
 */

class XMLParser_E : public herdstat_base_E
{
    public:
        typedef std::string string_type;

        XMLParser_E() { }
        XMLParser_E(const string_type &f, const string_type &e)
            : _file(f), _error(e) { }
        virtual ~XMLParser_E() throw() { }
        virtual const string_type &file() const { return _file; }
        virtual const string_type &error() const { return _error; }

    protected:
        string_type _file;
        string_type _error;
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
            _init.remove_whitespace(true);
            _init.validate_xml(validate);
        }

        virtual ~XMLParser_T() { }

        /* parse the given filename */
        virtual void parse(const string_type &path)
        {
            if (not handler->parse_file(path.c_str()))
                throw XMLParser_E(path, handler->get_error_message());
        }

    protected:
        value_type *handler;

    private:
        static xml::init _init;
};

#endif

/* vim: set tw=80 sw=4 et : */
