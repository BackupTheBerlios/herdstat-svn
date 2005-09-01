/*
 * herdstat -- src/saxparser.hh
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

#ifndef _HAVE_SAXPARSER_HH
#define _HAVE_SAXPARSER_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string>
#include <memory>
#include <map>
#include <libxml/parser.h>
#include <libxml/SAX2.h>
#include "exceptions.hh"

class SAXParser_E : public herdstat_base_E
{
    public:
        typedef std::string string_type;

        SAXParser_E() { }
        SAXParser_E(const string_type &e, const string_type &f = "")
            : _file(f), _error(e) { }
        virtual ~SAXParser_E() throw() { }
        virtual const string_type &file() const { return _file; }
        virtual const string_type &error() const { return _error; }

    protected:
        string_type _file;
        string_type _error;
};

class SAXParser
{
    public:
        typedef unsigned int size_type;
        typedef std::map<std::string, std::string> attrs_type;

        SAXParser();
        virtual ~SAXParser();

        virtual void parse_file(const std::string &path);

    protected:
        virtual void start_document();
        virtual void end_document();
        virtual void start_element(const std::string &name,
                                   const attrs_type &attrs);
        virtual void end_element(const std::string &name);
        virtual void characters(const std::string &text);
        virtual void comment(const std::string &text);
        virtual void warning(const std::string &text);
        virtual void error(const std::string &text);
        virtual void fatal_error(const std::string &text);
        virtual void cdata(const std::string &text);

    private:
        friend class SAXCallback;
        virtual void parse();

        void init_context();
        void destroy_context();

        xmlParserCtxt *_context;
        std::auto_ptr<xmlSAXHandler> _handler;

        /* for libxml2 initialization */
        class init
        {
            public:
                init();
                ~init();
        };

        static init _init;
};

#endif /* _HAVE_SAXPARSER_HH */

/* vim: set tw=80 sw=4 et : */
