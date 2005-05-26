/*
 * herdstat -- src/pkgcache.hh
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

#ifndef HAVE_PKGCACHE_HH
#define HAVE_PKGCACHE_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <map>
#include "common.hh"
#include "herds_xml.hh"

#define PKGCACHE    LOCALSTATEDIR"/pkgcache.xml"

/*
 * Container for package query data.
 */

class pkgQuery_T : public std::map<util::string, util::string>
{
    public:
        pkgQuery_T(const opts_type::value_type &n)
            : info(n), query(n) { }

        herds_xml_T::devinfo_T info;
        util::string query;
        int id;
        std::time_t date;
};

/*
 * Represents a cache of package query
 * results (produced by action_pkg_handler_T).
 */

//class pkgCache_T : public util::cache_T<std::vector<pkgQuery_T> >
class pkgCache_T : public std::vector<pkgQuery_T>
{
    public:
//        pkgCache_T() : util::cache_T<value_type>(PKGCACHE) { }
        virtual ~pkgCache_T() { }

//        void push_back(const pkgQuery_T &query)
//        { this->_cache.push_back(query); }
        iterator find(const util::string &);

//        virtual void init() { }
        virtual bool valid() const { return util::is_file(PKGCACHE); }
        virtual void read();
        virtual void write() const;
//        virtual void fill() { }

        bool exists(const util::string &);
};

#endif

/* vim: set tw=80 sw=4 et : */
