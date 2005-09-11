/*
 * herdstat -- portage/xml.hh
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

#ifndef _HAVE_XML_HH
#define _HAVE_XML_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string>
#include <herdstat/parsable.hh>
#include <herdstat/noncopyable.hh>
#include <herdstat/xml/saxparser.hh>

namespace portage {

    class Developer;

    /**
     * Abstract base class for portage-specific XML files.
     */

    class xmlBase : public parsable,
                    protected xml::saxhandler,
                    private noncopyable
    {
        public:
            xmlBase() : parsable() { }
            xmlBase(const std::string& path) : parsable(path) { }
            virtual ~xmlBase() { }

            virtual void parse(const std::string& path = "") = 0;
            virtual void fill_developer(Developer& dev) const = 0;

        protected:
            /* SAX2 callbacks */
            virtual bool start_element(const std::string& name,
                                       const attrs_type&  attrs) = 0;
            virtual bool end_element(const std::string& name) = 0;
            virtual bool text(const std::string& text) = 0;
    };

} // namespace portage

#endif /* _HAVE_XML_HH */

/* vim: set tw=80 sw=4 et : */
