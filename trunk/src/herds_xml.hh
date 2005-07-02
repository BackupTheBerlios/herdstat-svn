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

#include "xml.hh"
#include "herds_xml_handler.hh"

#define HERDS_XML_EXPIRE            86400

/*
 * Represents a herds.xml instance.
 */

class herds_xml_T : public xml_T<HerdsXMLHandler_T>
{
    public:
        typedef handler_type::herds_type herds_type;
        typedef handler_type::herd_type  herd_type;
        typedef handler_type::dev_type   dev_type;
        typedef herds_type::string_type string_type;
        typedef herds_type::iterator iterator;
        typedef herds_type::const_iterator const_iterator;
        typedef herds_type::size_type size_type;

        herds_xml_T()
            : xml_T<handler_type>(optget("qa", bool)), _fetched(false),
              _local_default(optget("localstatedir", util::string)+"/herds.xml")
        { this->init(); }
        virtual ~herds_xml_T() { }

        /* herd_type subset */
        iterator begin() { return this->_handler->herds.begin(); }
        const_iterator begin() const { return this->_handler->herds.begin(); }
        iterator end() { return this->_handler->herds.end(); }
        const_iterator end() const { return this->_handler->herds.end(); }
        iterator find(const string_type &s)
        { return this->_handler->herds.find(s); }
        const_iterator find(const string_type &s) const
        { return this->_handler->herds.find(s); }
        herd_type * &operator[] (const string_type &s)
        { return this->_handler->herds[s]; }
        herd_type * &operator[] (const string_type &s) const
        { return this->_handler->herds[s]; }
        size_type size() const { return this->_handler->herds.size(); }

        /* interface */
        herds_type &herds() const { return this->_handler->herds; }
        bool exists(const string_type &h) const
        { return this->find(h) != this->end(); }
        void display(std::ostream &s)
        { return this->_handler->herds.display(s); }

        struct devinfo_T
        {
            devinfo_T(const util::string &u) : user(u) { }
            opts_type herds;
            util::string name;
            util::string user;
        };

        const devinfo_T get_dev_info(const string_type &) const;

        void fetch();

    protected:
        virtual void init();

    private:
        bool _fetched;
        static const string_type _remote_default;
        const string_type _local_default;
};

#endif

/* vim: set tw=80 sw=4 et : */
