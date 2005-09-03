/*
 * herdstat -- src/pkgcache_T.hh
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

#include <herdstat/cachable.hh>
#include "common.hh"

class pkgcache_T : public cachable
{
    public:
        typedef std::vector<std::string> container_type;
        typedef container_type::value_type value_type;
        typedef container_type::iterator iterator;
        typedef container_type::const_iterator const_iterator;
        typedef container_type::size_type size_type;

        pkgcache_T();
        pkgcache_T(const std::string &portdir);
        void init(const std::string &portdir);

        virtual bool valid() const;
        virtual void fill();
        virtual void load();
        virtual void dump();

        iterator begin();
        const_iterator begin() const;
        iterator end();
        const_iterator end() const;
        size_type size() const;
        bool empty() const;

        const std::vector<std::string>& pkgs() const;

    private:
        void push_back(const value_type &v);

        std::string _portdir;
        std::vector<std::string> _overlays;
        std::vector<std::string> _pkgs;
};

inline pkgcache_T::iterator
pkgcache_T::begin()
{
    return _pkgs.begin();
}

inline pkgcache_T::const_iterator
pkgcache_T::begin() const
{
    return _pkgs.begin();
}

inline pkgcache_T::iterator
pkgcache_T::end()
{
    return _pkgs.end();
}

inline pkgcache_T::const_iterator
pkgcache_T::end() const
{
    return _pkgs.end();
}

inline pkgcache_T::size_type
pkgcache_T::size() const
{
    return _pkgs.size();
}

inline bool
pkgcache_T::empty() const
{
    return _pkgs.empty();
}

inline void
pkgcache_T::push_back(const value_type &v)
{
    _pkgs.push_back(v);
}

inline const std::vector<std::string>&
pkgcache_T::pkgs() const
{
    return _pkgs;
}

#endif

/* vim: set tw=80 sw=4 et : */
