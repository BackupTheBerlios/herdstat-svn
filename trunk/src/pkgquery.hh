/*
 * herdstat -- src/pkgquery.hh
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

#ifndef HAVE_PKGQUERY_HH
#define HAVE_PKGQUERY_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <map>
#include <herdstat/portage/developer.hh>

/*
 * Container for package query data.
 */

enum query_type { QUERYTYPE_DEV, QUERYTYPE_HERD };

class pkgQuery
{
    public:
        typedef std::map<std::string, std::string> container_type;
        typedef container_type::value_type value_type;
        typedef container_type::key_type key_type;
        typedef container_type::mapped_type mapped_type;
        typedef container_type::iterator iterator;
        typedef container_type::const_iterator const_iterator;
        typedef container_type::size_type size_type;

        pkgQuery(const std::string &n,
                   const std::string &w = "",
                   bool dev = false);

        void dump(std::ostream &) const;
        bool operator== (const pkgQuery &) const;
        std::vector<std::string> pkgs() const;

        inline iterator begin();
        inline const_iterator begin() const;
        inline iterator end();
        inline const_iterator end() const;

        inline size_type size() const;
        inline bool empty() const;

        inline mapped_type& operator[] (const key_type& k);

        inline std::pair<iterator, bool> insert(const value_type& v);
        template <class In> inline void insert(In begin, In end);

        inline void erase(iterator pos);
        inline void erase(iterator begin, iterator end);
        inline size_type erase(const key_type& k);

        inline iterator find(const key_type& k);
        inline const_iterator find(const key_type& k) const;

        container_type _pkgs;
        herdstat::portage::Developer info;
        std::string query, with, portdir;
        std::vector<std::string> overlays;
        std::time_t date;
        query_type type;
};

inline pkgQuery::iterator pkgQuery::begin() { return _pkgs.begin(); }
inline pkgQuery::const_iterator pkgQuery::begin() const { return _pkgs.begin(); }
inline pkgQuery::iterator pkgQuery::end() { return _pkgs.end(); }
inline pkgQuery::const_iterator pkgQuery::end() const { return _pkgs.end(); }
inline pkgQuery::size_type pkgQuery::size() const { return _pkgs.size(); }
inline bool pkgQuery::empty() const { return _pkgs.empty(); }
inline pkgQuery::mapped_type& pkgQuery::operator[] (const key_type& k)
{ return _pkgs[k]; }
inline void pkgQuery::erase(iterator pos) { _pkgs.erase(pos); }
inline void pkgQuery::erase(iterator begin, iterator end) { _pkgs.erase(begin, end); }
inline pkgQuery::size_type pkgQuery::erase(const key_type& k)
{ return _pkgs.erase(k); }
inline pkgQuery::iterator pkgQuery::find(const key_type& k)
{ return _pkgs.find(k); }
inline pkgQuery::const_iterator pkgQuery::find(const key_type& k) const
{ return _pkgs.find(k); }
inline std::pair<pkgQuery::iterator, bool>
pkgQuery::insert(const value_type& v) { return _pkgs.insert(v); }
template <class In>
inline void pkgQuery::insert(In begin, In end) { _pkgs.insert(begin, end); }

#endif

/* vim: set tw=80 sw=4 fdm=marker et : */
