/*
 * herdstat -- src/herds_xml_handler.hh
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

#ifndef HAVE_HERDS_XML_HANDLER_HH
#define HAVE_HERDS_XML_HANDLER_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "herds.hh"
#include "xmlparser.hh"

/*
 * Content Handler for herds.xml
 */

class HerdsXMLHandler_T : public XMLHandler_T
{
    private:
        /* internal state variables */
        bool in_herd;
        bool in_herd_name;
        bool in_herd_email;
        bool in_herd_desc;
        bool in_maintainer;
        bool in_maintainer_name;
        bool in_maintainer_email;
        bool in_maintainer_role;

        std::string cur_herd;
        std::string cur_dev;

    protected:
        /* callbacks */
        virtual bool start_element(const std::string &, const attrs_type &);
        virtual bool end_element(const std::string &);
        virtual bool text(const std::string &);

    public:
        typedef std::map<std::string, std::string>  descs_T;

        HerdsXMLHandler_T()
        {
            in_herd = in_herd_name = in_herd_email = in_herd_desc =
            in_maintainer = in_maintainer_name = in_maintainer_email =
            in_maintainer_role = false;
        }
        virtual ~HerdsXMLHandler_T();

        herds_T herds;
};

#endif

/* vim: set tw=80 sw=4 et : */
