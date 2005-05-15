/*
 * herdstat -- src/metadata_xml.hh
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

#ifndef HAVE_METADATA_XML_HH
#define HAVE_METADATA_XML_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "xml.hh"
#include "metadata_xml_handler.hh"

/*
 * Represents a metadata.xml file
 */

class metadata_xml_T : public xml_T<MetadataXMLHandler_T>
{
    public:
        typedef handler_type::herds_type    herds_type;
        typedef handler_type::herd_type     herd_type;
        typedef util::string                string_type;

        typedef herds_type::iterator        herds_iterator;
        typedef herds_type::const_iterator  const_herds_iterator;
        typedef herds_type::size_type       herds_size_type;

        typedef herd_type::iterator         herd_iterator;
        typedef herd_type::const_iterator   const_herd_iterator;
        typedef herd_type::size_type        herd_size_type;

        metadata_xml_T(const string_type &s)
            : xml_T<handler_type>(s, optget("qa", bool)) { }
        virtual ~metadata_xml_T() { }

        /* handler_type access functions */
        herds_type  &herds()    const { return this->_handler->herds; }
        herd_type   &devs()     const { return this->_handler->devs;  }
        string_type &longdesc() const { return this->_handler->longdesc; }

        bool is_category() const { return this->_handler->is_category; }
        bool dev_exists(const herd_type::key_type &d) const;
        bool herd_exists(const herds_type::value_type &h) const;
};

#endif

/* vim: set tw=80 sw=4 et : */
