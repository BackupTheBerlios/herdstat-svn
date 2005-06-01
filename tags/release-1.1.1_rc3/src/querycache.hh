/*
 * herdstat -- src/querycache.hh
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

#ifndef HAVE_QUERYCACHE_HH
#define HAVE_QUERYCACHE_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <map>
#include "common.hh"
#include "pkgquery.hh"
#include "herds_xml.hh"

/*
 * Represents a cache of package query
 * results (produced by action_pkg_handler_T::search()).
 */

class querycache_T : public util::cache_T<std::vector<pkgQuery_T> >
{
    public:
        querycache_T();
        virtual ~querycache_T() { }

        void operator() (const pkgQuery_T &);
        virtual bool valid() const;
        virtual void load();
        virtual void dump();
        void dump(std::ostream &);

        iterator find(const pkgQuery_T &);
        pkgQuery_T &front() { return this->_cache.front(); }
        pkgQuery_T &back() { return this->_cache.back(); }
        void push_back(const pkgQuery_T &q) { this->_cache.push_back(q); }

        bool is_expired(const pkgQuery_T &) const;
        void sort_oldest_to_newest();
        std::vector<util::string> queries() const;

    protected:
        void purge_old();
};

#endif

/* vim: set tw=80 sw=4 et : */
