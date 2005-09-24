/*
 * herdstat -- portage/herds_xml.hh
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

#ifndef _HAVE_HERDS_XML_HH
#define _HAVE_HERDS_XML_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/**
 * @file herds_xml.hh
 * @brief Defines the interface to herds.xml.
 */

#include <algorithm>
#include <herdstat/fetcher.hh>
#include <herdstat/portage/xmlbase.hh>
#include <herdstat/portage/herd.hh>

namespace portage {

    class herds_xml : public xmlBase
    {
        public:
            herds_xml();
            herds_xml(const std::string& path);
            virtual ~herds_xml();

            virtual void parse(const std::string& path = "");
            virtual void fill_developer(Developer& dev) const;

            inline void set_cvsdir(const std::string& path);
            inline void set_force_fetch(bool force);

            /// Implicit conversion to Herds::container_type
            inline operator Herds::container_type() const;

            inline const Herds& herds() const;
            inline Herds& herds();

            /* convienence */
            inline Herds::size_type size() const;
            inline bool empty() const;

        protected:
            virtual bool start_element(const std::string &name,
                                       const attrs_type  &attrs);
            virtual bool end_element(const std::string &name);
            virtual bool text(const std::string &text);

        private:
            Herds _herds;
            std::string _cvsdir;
            bool _force_fetch;
            Fetcher _fetch; /* for fetching <maintainingproject> XML's */
            static const char * const _local_default;

            /* internal state variables */
            bool in_herd,
                 in_herd_name,
                 in_herd_email,
                 in_herd_desc,
                 in_maintainer,
                 in_maintainer_name,
                 in_maintainer_email,
                 in_maintainer_role,
                 in_maintaining_prj;

            Herds::iterator _cur_herd;
            Herd::iterator  _cur_dev;
    };

    inline herds_xml::operator Herds::container_type() const { return _herds; }
    inline const Herds& herds_xml::herds() const { return _herds; }
    inline Herds& herds_xml::herds() { return _herds; }
    inline bool herds_xml::empty() const { return _herds.empty(); }
    inline Herds::size_type herds_xml::size() const { return _herds.size(); }
    inline void herds_xml::set_cvsdir(const std::string& path) { _cvsdir.assign(path); }
    inline void herds_xml::set_force_fetch(bool v) { _force_fetch = v; }

} // namespace portage

#endif /* _HAVE_HERDS_XML_HH */

/* vim: set tw=80 sw=4 et : */
