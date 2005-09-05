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

#include <herdstat/xml/saxparser.hh>
#include "herds.hh"
#include "metadata.hh"

/*
 * Content Handler for metadata.xml
 */

class MetadataXMLHandler_T : public xml::saxhandler
{
    public:
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

        virtual ~MetadataXMLHandler_T() { }

        metadata_T data;

    protected:
        /* callbacks */
        virtual bool start_element(const std::string &, const attrs_type &);
        virtual bool end_element(const std::string &);
        virtual bool text(const std::string &);

    private:
        /* internal state variables */
        bool in_herd;
        bool in_maintainer;
        bool in_email;
        bool in_name;
        bool in_desc;
        bool in_longdesc;

        std::string cur_dev;
};

#endif

/* vim: set tw=80 sw=4 et : */
