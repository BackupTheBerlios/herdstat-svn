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

/*
 * Represents a package query results
 * (produced by action_pkg_handler_T::search()).
 */

class querycache_T
{
    public:
        typedef std::vector<pkgQuery_T> container_type;
        typedef container_type::value_type value_type;
        typedef container_type::reference reference;
        typedef container_type::const_reference const_reference;
        typedef container_type::iterator iterator;
        typedef container_type::const_iterator const_iterator;
        typedef container_type::size_type size_type;

        querycache_T();

        void operator() (const pkgQuery_T &);
        void load();
        void dump();
        void dump(std::ostream &);

        bool is_expired(const pkgQuery_T &) const;
        void sort_oldest_to_newest();
        std::vector<std::string> queries() const;

        iterator begin();
        const_iterator begin() const;
        iterator end();
        const_iterator end() const;
        reference front();
        const_reference front() const;
        reference back();
        const_reference back() const;
        iterator find(const_reference q);
        const_iterator find(const_reference q) const;
        size_type size() const;
        bool empty() const;
        void clear();

    private:
        void purge_old();

        container_type _queries;
        int _max;
        long _expire;
        const std::string _path;
};

inline querycache_T::iterator querycache_T::begin() { return _queries.begin(); }
inline querycache_T::const_iterator querycache_T::begin() const
{ return _queries.begin(); }
inline querycache_T::iterator querycache_T::end() { return _queries.end(); }
inline querycache_T::const_iterator querycache_T::end() const
{ return _queries.end(); }
inline querycache_T::reference querycache_T::front() { return _queries.front(); }
inline querycache_T::const_reference querycache_T::front() const
{ return _queries.front(); }
inline querycache_T::reference querycache_T::back() { return _queries.back(); }
inline querycache_T::const_reference querycache_T::back() const
{ return _queries.back(); }
inline querycache_T::iterator querycache_T::find(const_reference q)
{ return std::find(_queries.begin(), _queries.end(), q); }
inline querycache_T::const_iterator querycache_T::find(const_reference q) const
{ return std::find(_queries.begin(), _queries.end(), q); }
inline querycache_T::size_type  querycache_T::size() const { return _queries.size(); }
inline bool querycache_T::empty() const { return _queries.empty(); }
inline void querycache_T::clear() { _queries.clear(); }

inline bool querycache_T::is_expired(const_reference q) const
{ return ((std::time(NULL) - q.date) > this->_expire); } 

#endif

/* vim: set tw=80 sw=4 et : */
