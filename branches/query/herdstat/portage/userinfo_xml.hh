/*
 * herdstat -- portage/userinfo_xml.hh
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

#ifndef _HAVE_USERINFO_XML_HH
#define _HAVE_USERINFO_XML_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/**
 * @file userinfo_xml.hh
 * @brief Defines the interface to Gentoo's userinfo.xml.
 */

#include <herdstat/portage/xmlbase.hh>
#include <herdstat/portage/herd.hh>

namespace portage {

    /**
     * Represents Gentoo's userinfo.xml.
     */

    class userinfo_xml : public xmlBase
    {
        public:
            userinfo_xml();
            userinfo_xml(const std::string& path);
            virtual ~userinfo_xml();

            virtual void parse(const std::string& path = "");
            virtual void fill_developer(Developer& dev) const;

            inline const Developers& devs() const;

            /* convenience */
            inline Developers::size_type size() const;
            inline bool empty() const;

        protected:
            virtual bool start_element(const std::string& name,
                                       const attrs_type& attrs);
            virtual bool end_element(const std::string& name);
            virtual bool text(const std::string& text);

        private:
            Developers _devs;
            static const char * const _local_default;

            bool in_user,
                 in_firstname,
                 in_familyname,
                 in_pgpkey,
                 in_email,
                 in_joined,
                 in_birth,
                 in_roles,
                 in_status,
                 in_location;

            Developers::iterator _cur_dev;
    };

    inline const Developers& userinfo_xml::devs() const { return _devs; }
    inline Developers::size_type userinfo_xml::size() const { return _devs.size(); }
    inline bool userinfo_xml::empty() const { return _devs.empty(); }

} // namespace portage

#endif /* _HAVE_USERINFO_XML_HH */

/* vim: set tw=80 sw=4 et : */
