/*
 * herdstat -- portage/herds_xml.hh
 * $Id: herds_xml.hh 520 2005-09-05 11:59:58Z ka0ttic $
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
#include <herdstat/fetchable.hh>
#include <herdstat/parsable.hh>
#include <herdstat/xml/saxparser.hh>
#include <herdstat/portage/herd.hh>

namespace portage {

    class herds_xml : public fetchable,
                      public parsable,
                      protected xml::saxhandler
    {
        public:
            typedef Herds::value_type value_type;
            typedef Herds::iterator iterator;
            typedef Herds::const_iterator const_iterator;
            typedef Herds::size_type size_type;

            herds_xml();
            herds_xml(const std::string& path);
            virtual ~herds_xml();

            virtual void parse(const std::string& path = "");
            virtual void fetch() const;

            /// Implicit conversion to std::vector<herd>.
            operator std::vector<Herd>() const;

            /* herds subset */
            iterator begin();
            const_iterator begin() const;
            iterator end();
            const_iterator end() const;
            size_type size() const;
            bool empty() const;
            iterator find(const std::string& herd);
            const_iterator find(const std::string& herd) const;
            iterator find(const Herd& h);
            const_iterator find(const Herd& h) const;

            bool exists(const std::string& herd) const;

        protected:
            virtual bool start_element(const std::string &name,
                                       const attrs_type  &attrs);
            virtual bool end_element(const std::string &name);
            virtual bool text(const std::string &text);

        private:
            Herds _herds;
            static const char * const _local_default;
            static const char * const _remote_default;
            mutable bool _fetched;

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

    inline herds_xml::operator std::vector<Herd>() const { return _herds; }
    inline herds_xml::iterator herds_xml::begin() { return _herds.begin(); }
    inline herds_xml::const_iterator herds_xml::begin() const
    { return _herds.begin(); }
    inline herds_xml::iterator herds_xml::end() { return _herds.end(); }
    inline herds_xml::const_iterator herds_xml::end() const
    { return _herds.end(); }
    inline herds_xml::size_type herds_xml::size() const { return _herds.size(); }
    inline bool herds_xml::empty() const { return _herds.empty(); }
    inline herds_xml::iterator herds_xml::find(const std::string& herd)
    { return _herds.find(herd); }
    inline herds_xml::const_iterator herds_xml::find(const std::string& herd) const
    { return _herds.find(herd); }
    inline herds_xml::iterator herds_xml::find(const Herd& h)
    { return _herds.find(h); }
    inline herds_xml::const_iterator herds_xml::find(const Herd& h) const
    { return _herds.find(h); }
    inline bool herds_xml::exists(const std::string& herd) const
    { return (this->find(herd) != this->end()); }

} // namespace portage

#endif /* _HAVE_HERDS_XML_HH */

/* vim: set tw=80 sw=4 et : */
