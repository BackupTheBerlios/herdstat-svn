/*
 * herdstat -- src/metadata_xml_handler.hh
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

#ifndef HAVE_METADATA_XML_HANDLER_HH
#define HAVE_METADATA_XML_HANDLER_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "herds.hh"
#include "xmlparser.hh"

/*
 * Content Handler for metadata.xml
 */

class MetadataXMLHandler_T : public XMLHandler_T
{
    public:
        typedef std::vector<util::string> herds_type;
        typedef herd_T herd_type;
        typedef dev_attrs_T dev_type;

        MetadataXMLHandler_T()
        {
            in_herd = 
            in_maintainer =
            in_email =
            in_name =
            in_desc =
            in_longdesc = false;
        }
        virtual ~MetadataXMLHandler_T();

        herds_type herds;
        herd_type devs;
        util::string longdesc;

    protected:
        /* callbacks */
        virtual return_type
        START_ELEMENT(const string_type &, const attrs_type &);
        virtual return_type END_ELEMENT(const string_type &);
        virtual return_type CHARACTERS(const string_type &);

    private:
        /* internal state variables */
        bool in_herd;
        bool in_maintainer;
        bool in_email;
        bool in_name;
        bool in_desc;
        bool in_longdesc;

        string_type cur_dev;
};

#endif

/* vim: set tw=80 sw=4 et : */
