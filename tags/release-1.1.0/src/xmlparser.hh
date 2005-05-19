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

#ifdef UNICODE
# define USE_LIBXMLPP
#else /* UNICODE */
# define USE_XMLWRAPP
#endif /* UNICODE */

#include <istream>

#ifdef USE_LIBXMLPP
# include <libxml++/libxml++.h>
#else /* USE_LIBXMLPP */
# include <xmlwrapp/init.h>
# include <xmlwrapp/event_parser.h>
#endif /* USE_LIBXMLPP */

#include "common.hh"

/* callback names - XML parser implementation-dependant */
#ifdef USE_LIBXMLPP
# define START_ELEMENT   on_start_element
# define END_ELEMENT     on_end_element
# define CHARACTERS      on_characters
#else /* USE_LIBXMLPP */
# define START_ELEMENT  start_element
# define END_ELEMENT    end_element
# define CHARACTERS     text
#endif /* USE_LIBXMLPP */

/*
 * Abstract XML Content Handler
 */

#ifdef USE_LIBXMLPP
class XMLHandler_T : public xmlpp::SaxParser
#else
class XMLHandler_T : public xml::event_parser
#endif /* USE_LIBXMLPP */
{
    protected:
#ifdef USE_LIBXMLPP
        typedef Glib::ustring string_type;
        typedef AttributeList attrs_type;
        typedef void return_type;
#else /* USE_LIBXMLPP */
        typedef std::string string_type;
        typedef bool return_type;
#endif /* USE_LIBXMLPP */

        virtual return_type
        START_ELEMENT(const string_type &, const attrs_type &) = 0;
        virtual return_type END_ELEMENT(const string_type &) = 0;
        virtual return_type CHARACTERS(const string_type &) = 0;
};

/*
 * parser exception class
 */

class XMLParser_E : public herdstat_base_E
{
    public:
        typedef util::string string_type;

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
        typedef util::path_T string_type;

        XMLParser_T(value_type *h, bool validate = false) : handler(h)
        {
#ifdef USE_LIBXMLPP
            handler->set_validate(validate);
#else /* USE_LIBXMLPP */
            init.remove_whitespace(true);
            init.validate_xml(validate);
#endif /* USE_LIBXMLPP */
        }

        virtual ~XMLParser_T() { }

        /* parse the given filename */
        virtual void parse(const string_type &path)
        {
#ifdef USE_LIBXMLPP
            try { handler->parse_file(path); }
            catch (const xmlpp::exception &e)
            { throw XMLParser_E(path, e.what()); }
#else /* USE_LIBXMLPP */
            if (not handler->parse_file(path.c_str()))
                throw XMLParser_E(path, handler->get_error_message());
#endif /* USE_LIBXMLPP */
        }

        /* parse the given open stream */
        virtual void parse(std::istream &stream,
                           const string_type &path = "stream")
        {
#ifdef USE_LIBXMLPP
            try { handler->parse_stream(stream); }
            catch (const xmlpp::exception &e)
            { throw XMLParser_E(path, e.what()); }
#else /* USE_LIBXMLPP */
            if (not handler->parse_stream(stream))
                throw XMLParser_E(path, handler->get_error_message());
#endif /* USE_LIBXMLPP */
        }

    protected:
#ifdef USE_XMLWRAPP
        xml::init init;
#endif

        value_type *handler;
};

#endif

/* vim: set tw=80 sw=4 et : */
