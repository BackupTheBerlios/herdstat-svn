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
#include <string>
#include <xmlwrapp/init.h>
#include <xmlwrapp/event_parser.h>
#include "exceptions.hh"

class XMLHandler_T : public xml::event_parser
{
    protected:
        virtual bool start_element(const std::string &, const attrs_type &) = 0;
        virtual bool end_element(const std::string &) = 0;
        virtual bool text(const std::string &) = 0;
};

class XMLParser_T
{
    protected:
        xml::init xmlinit;
        XMLHandler_T *handler;

    public:
        XMLParser_T(XMLHandler_T *h) : handler(h)
        {
            xmlinit.remove_whitespace(true);
        }

        virtual ~XMLParser_T() { }

        virtual void parse(const std::string &f)
        {
            if (not handler->parse_file(f.c_str()))
                throw XMLParser_E(f, handler->get_error_message());
        }

        virtual void parse(std::istream &stream)
        {
            if (not handler->parse_stream(stream))
                throw XMLParser_E("stream", handler->get_error_message());
        }
};

#endif

/* vim: set tw=80 sw=4 et : */
