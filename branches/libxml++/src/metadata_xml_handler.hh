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
    private:
        /* internal state variables */
        bool in_herd;
        bool in_maintainer;
        bool in_email;
        bool in_name;
        bool in_desc;
        bool in_longdesc;
        bool found_lang;

        util::string cur_dev;

    protected:
        /* callbacks */
        virtual void on_start_element(const Glib::ustring &,
                                      const AttributeList &);
        virtual void on_end_element(const Glib::ustring &);
        virtual void on_characters(const Glib::ustring &);

    public:
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

        std::vector<util::string> herds;
        herd_T devs;
        util::string longdesc;
};

#endif

/* vim: set tw=80 sw=4 et : */
