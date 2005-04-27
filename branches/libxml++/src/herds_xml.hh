/*
 * herdstat -- src/herds_xml.hh
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

#ifndef HAVE_HERDS_XML_HH
#define HAVE_HERDS_XML_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <memory>
#include "common.hh"
#include "herds.hh"
#include "herds_xml_handler.hh"

#define HERDS_XML_FETCH_LOCATION    LOCALSTATEDIR"/herds.xml"
#define HERDS_XML_EXPIRE            86400

/*
 * Represents a herds.xml.
 */

class herds_xml_T
{
    private:
        void init();
        void fetch();
        void parse();

        const bool _fetchonly;
        util::path_T _path;
        static const util::string _default;
        const std::auto_ptr<HerdsXMLHandler_T> _handler;
        util::timer_T _timer;

    public:
        typedef herds_T::iterator iterator;
        typedef herds_T::const_iterator const_iterator;
        typedef herds_T::size_type size_type;

        herds_xml_T(bool f = false) : _fetchonly(f), _path(_default),
            _handler(new HerdsXMLHandler_T())
        { this->init(); }

        /* herds_T subset */
        iterator begin() { return this->_handler->herds.begin(); }
        const_iterator begin() const { return this->_handler->herds.begin(); }
        iterator end() { return this->_handler->herds.end(); }
        const_iterator end() const { return this->_handler->herds.end(); }
        iterator find(const util::string &s)
        { return this->_handler->herds.find(s); }
        const_iterator find(const util::string &s) const
        { return this->_handler->herds.find(s); }
        herd_T * &operator[] (const util::string &s)
        { return this->_handler->herds[s]; }
        size_type size() const { return this->_handler->herds.size(); }

        herds_T &herds() const { return this->_handler->herds; }
        bool exists(const util::string &h) const
        { return this->find(h) != this->end(); }
        void display(std::ostream &s)
        { return this->_handler->herds.display(s); }
};

#endif

/* vim: set tw=80 sw=4 et : */
