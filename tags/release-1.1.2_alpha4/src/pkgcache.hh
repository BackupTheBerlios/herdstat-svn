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
        void init();

        virtual bool valid() const;
        virtual void fill();
        virtual void load();
        virtual void dump();

        /// Implicit conversion to container_type
        inline operator const container_type&() const;

        inline iterator begin();
        inline const_iterator begin() const;
        inline iterator end();
        inline const_iterator end() const;
        inline size_type size() const;
        inline bool empty() const;

    private:
        const std::string& _portdir;
        const std::vector<std::string>& _overlays;
        container_type _pkgs;
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

inline pkgcache_T::operator
const pkgcache_T::container_type&() const
{
    return _pkgs;
}

#endif

/* vim: set tw=80 sw=4 et : */
