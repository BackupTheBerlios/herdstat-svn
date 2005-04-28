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
#include <libxml++/libxml++.h>
#include "common.hh"

/*
 * Abstract XML Content Handler
 */

class XMLHandler_T : public xmlpp::SaxParser
{
    protected:
        virtual void on_start_element(const Glib::ustring &,
                                      const AttributeList &) = 0;
        virtual void on_end_element(const Glib::ustring &) = 0;
        virtual void on_characters(const Glib::ustring &) = 0;
};

/*
 * parser exception class
 */

class XMLParser_E : public herdstat_base_E
{
    protected:
        util::string _file;
        util::string _error;

    public:
        XMLParser_E() { }
        XMLParser_E(const util::string &f, const util::string &e)
            : _file(f), _error(e) { }
        virtual ~XMLParser_E() throw() { }
        virtual const util::string &file() const { return _file; }
        virtual const util::string &error() const { return _error; }
};

/*
 * XML Parser - takes a pointer to a XMLHandler_T object
 */

class XMLParser_T
{
    protected:
        XMLHandler_T *handler;

    public:
        XMLParser_T(XMLHandler_T *h, bool validate = false) : handler(h)
        {
            handler->set_validate(validate);
        }

        virtual ~XMLParser_T() { }

        /* parse the given filename */
        virtual void parse(const util::path_T &path)
        {
            try
            {
                handler->parse_file(path);
            }
            catch (const xmlpp::exception &e)
            {
                throw XMLParser_E(path, e.what());
            }
        }

        /* parse the given open stream */
        virtual void parse(std::istream &stream,
                           const util::path_T path = "stream")
        {
            try
            {
                handler->parse_stream(stream);
            }
            catch (const xmlpp::exception &e)
            {
                throw XMLParser_E(path, e.what());
            }
        }
};

#endif

/* vim: set tw=80 sw=4 et : */
