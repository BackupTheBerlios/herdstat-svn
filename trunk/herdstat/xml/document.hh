/*
 * herdstat -- src/xml.hh
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

#ifndef HAVE_XML_HH
#define HAVE_XML_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <memory>
#include <herdstat/parsable.hh>
#include <herdstat/util/file.hh>
#include <herdstat/xml/exceptions.hh>
#include <herdstat/xml/saxparser.hh>

namespace xml {

    /*
     * Represents an XML document.
     */

    template <class H>
    class Document : public parsable
    {
        public:
            typedef H handler_type;

            Document();
            Document(const std::string &p);
            virtual ~Document() { }

            H *handler() const { return this->_handler.get(); }

            virtual void parse(const std::string &p = "");

        private:
            const std::auto_ptr<H> _handler;    /* content handler */
    };

    template <class H>
    Document<H>::Document() : parsable(), _handler(new H()) { }

    template <class H>
    Document<H>::Document(const std::string &path)
    : parsable(path), _handler(new H())
    {
        if (not util::is_file(path))
            throw FileException(path);

        this->parse();
    }

    template <class H>
    void
    Document<H>::parse(const std::string &path)
    {
        const std::string file(path.empty() ? this->path() : path);
        saxparser p(this->_handler.get());
        this->timer().start();
        p.parse(file);
        this->timer().stop();
    }

} // namespace xml
#endif

/* vim: set tw=80 sw=4 et : */
