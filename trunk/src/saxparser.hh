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

#include <memory>

extern "C"
{
    struct xmlSAXHandler;
}

class SAXParser
{
    public:
        typedef unsigned int size_type;
        typedef std::map<std::string, std::string> attrs_type;

        SAXParser();
        virtual ~SAXParser();

        virtual void parse_file(const std::string &path);
        virtual void parse_memory(const std::string &contents);
        virtual void parse_stream(std::istream &stream);
        virtual void parse_chunk(const std::string &chunk);
        virtual void finish_chunk();

        void parse_memory_raw(const unsigned char *contents, size_type bytes);

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

        xmlParserCtxt *_context;
        std::auto_ptr<xmlSAXHandler> _handler;
};

#endif /* _HAVE_SAXPARSER_HH */

/* vim: set tw=80 sw=4 et : */
