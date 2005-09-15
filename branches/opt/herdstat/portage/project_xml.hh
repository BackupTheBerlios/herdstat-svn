/*
 * herdstat -- herdstat/portage/project_xml.hh
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

#ifndef _HAVE_PROJECT_XML_HH
#define _HAVE_PROJECT_XML_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/**
 * @file project_xml.hh
 * @brief Defines the interface for Gentoo projectxml files.
 */

#include <herdstat/fetchable.hh>
#include <herdstat/portage/xmlbase.hh>
#include <herdstat/portage/herd.hh>

namespace portage {

    class project_xml : public xmlBase,
                        public fetchable
    {
        public:
//            project_xml();
            project_xml(const std::string& path,
                        const std::string& cvsdir,
                        const std::string& herd);
            virtual ~project_xml();

            virtual void parse(const std::string& path = "");
            virtual void fill_developer(Developer& dev) const { }

            const Herd& devs() const;

//            static const char * const baseURL() const;
//            static const char * const baseLocal() const;

        protected:
            virtual bool start_element(const std::string& name,
                                       const attrs_type&  attrs);
            virtual bool end_element(const std::string& name);
            virtual bool text(const std::string& text);

            virtual void do_fetch(const std::string& path = "") const
                throw (FetchException);

        private:
            Herd _devs;
            const std::string& _cvsdir;
            const std::string& _herd_name;
            bool in_sub, in_dev, in_task;
            std::string _cur_role, _cur_sub;
            static const char * const _baseURL;
            static const char * const _baseLocal;
    };

    inline const Herd& project_xml::devs() const { return _devs; }

} // namespace portage

#endif /* _HAVE_PROJECT_XML_HH */

/* vim: set tw=80 sw=4 et : */
