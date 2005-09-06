/*
 * herdstat -- portage/devaway_xml.hh
 * $Id: devaway_xml.hh 520 2005-09-05 11:59:58Z ka0ttic $
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

#ifndef _HAVE_DEVAWAY_XML_HH
#define _HAVE_DEVAWAY_XML_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/**
 * @file devaway_xml.hh
 * @brief Defines the interface for Gentoo's devaway.xml.
 */

#include <herdstat/parsable.hh>
#include <herdstat/fetchable.hh>
#include <herdstat/xml/saxparser.hh>
#include <herdstat/portage/herd.hh>

namespace portage {

    /**
     * Represents Gentoo's devaway.xml.
     */

    class devaway_xml : public parsable,
                        public fetchable,
                        protected xml::saxhandler
    {
        public:
            devaway_xml();
            devaway_xml(const std::string& path);
            virtual ~devaway_xml();

            virtual void fetch() const;
            virtual void parse(const std::string& path = "");

            const Herd& devs() const;

        protected:
            virtual bool start_element(const std::string& name,
                                       const attrs_type& attrs);
            virtual bool end_element(const std::string& name);
            virtual bool text(const std::string& text);

        private:
            Herd _devs;
            mutable bool _fetched;
            static const char * const _local_default;
            static const char * const _remote_default;

            bool in_devaway,
                 in_dev,
                 in_reason;

            Herd::iterator _cur_dev;
    };

    inline const Herd& devaway_xml::devs() const { return _devs; }

} // namespace portage

#endif /* _HAVE_DEVAWAY_XML_HH */

/* vim: set tw=80 sw=4 et : */