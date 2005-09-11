/*
 * herdstat -- portage/metadata_xml.hh
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

#ifndef _HAVE_METADATA_XML_HH
#define _HAVE_METADATA_XML_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/**
 * @file metadata_xml.hh
 * @brief Defines the interface for metadata.xml files.
 */

#include <herdstat/parsable.hh>
#include <herdstat/xml/saxparser.hh>
#include <herdstat/portage/metadata.hh>

namespace portage {

    class metadata_xml : public parsable,
                         protected xml::saxhandler
    {
        public:
            metadata_xml();
            metadata_xml(const std::string& path);
            virtual ~metadata_xml();

            virtual void parse(const std::string& path = "");

            /// Implicit conversion to metadata.
            operator metadata() const;

            /* for convenience */
            const std::string& longdesc() const;
            const Herds& herds() const;
            const Developers&  devs()  const;

        protected:
            virtual bool start_element(const std::string& name,
                                       const attrs_type& attrs);
            virtual bool end_element(const std::string& name);
            virtual bool text(const std::string& text);

        private:
            metadata _data;

            bool in_herd,
                 in_maintainer,
                 in_email,
                 in_name,
                 in_desc,
                 in_longdesc;

            Developers::iterator _cur_dev;
    };

    inline metadata_xml::operator metadata() const { return _data; }
    inline const Herds& metadata_xml::herds() const { return _data.herds(); }
    inline const Developers&  metadata_xml::devs()  const { return _data.devs(); }
    inline const std::string& metadata_xml::longdesc() const
    { return _data.longdesc(); }

} // namespace portage

#endif /* _HAVE_METADATA_XML_HH */

/* vim: set tw=80 sw=4 et : */
