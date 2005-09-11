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
#include "common.hh"

/*
 * Container for package query data.
 */

enum query_type { QUERYTYPE_DEV, QUERYTYPE_HERD };

class pkgQuery_T : public std::map<std::string, std::string>
{
    public:
        typedef std::map<std::string, std::string> container_type;
        typedef container_type::value_type value_type;
        typedef container_type::key_type key_type;
        typedef container_type::mapped_type mapped_type;
        typedef container_type::iterator iterator;
        typedef container_type::const_iterator const_iterator;
        typedef container_type::size_type size_type;

        pkgQuery_T(const std::string &n,
                   const std::string &w = "",
                   bool dev = false);

        void dump(std::ostream &) const;
        bool operator== (const pkgQuery_T &) const;
        std::vector<std::string> pkgs() const;

        iterator begin();
        const_iterator begin() const;
        iterator end();
        const_iterator end() const;

        size_type size() const;
        bool empty() const;

        mapped_type& operator[] (const key_type& k);

        std::pair<iterator, bool> insert(const value_type& v);
        template <class In> void insert(In begin, In end);

        void erase(iterator pos);
        void erase(iterator begin, iterator end);
        size_type erase(const key_type& k);

        iterator find(const key_type& k);
        const_iterator find(const key_type& k) const;

        container_type _pkgs;
        portage::Developer info;
        std::string query, with, portdir;
        std::vector<std::string> overlays;
        std::time_t date;
        query_type type;
};

inline pkgQuery_T::iterator pkgQuery_T::begin() { return _pkgs.begin(); }
inline pkgQuery_T::const_iterator pkgQuery_T::begin() const { return _pkgs.begin(); }
inline pkgQuery_T::iterator pkgQuery_T::end() { return _pkgs.end(); }
inline pkgQuery_T::const_iterator pkgQuery_T::end() const { return _pkgs.end(); }
inline pkgQuery_T::size_type pkgQuery_T::size() const { return _pkgs.size(); }
inline bool pkgQuery_T::empty() const { return _pkgs.empty(); }
inline pkgQuery_T::mapped_type& pkgQuery_T::operator[] (const key_type& k)
{ return _pkgs[k]; }
inline void pkgQuery_T::erase(iterator pos) { _pkgs.erase(pos); }
inline void pkgQuery_T::erase(iterator begin, iterator end) { _pkgs.erase(begin, end); }
inline pkgQuery_T::size_type pkgQuery_T::erase(const key_type& k)
{ return _pkgs.erase(k); }
inline pkgQuery_T::iterator pkgQuery_T::find(const key_type& k)
{ return _pkgs.find(k); }
inline pkgQuery_T::const_iterator pkgQuery_T::find(const key_type& k) const
{ return _pkgs.find(k); }
inline std::pair<pkgQuery_T::iterator, bool>
pkgQuery_T::insert(const value_type& v) { return _pkgs.insert(v); }
template <class In>
inline void pkgQuery_T::insert(In begin, In end) { _pkgs.insert(begin, end); }

#endif

/* vim: set tw=80 sw=4 et : */
