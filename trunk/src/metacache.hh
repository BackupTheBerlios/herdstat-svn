/*
 * herdstat -- src/metacache.hh
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

#ifndef HAVE_METACACHE_HH
#define HAVE_METACACHE_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <herdstat/cachable.hh>
#include <herdstat/portage/metadata.hh>

/*
 * A cache of all metadata.xml's.
 */

class metacache_T : public cachable
{
    public:
        typedef std::vector<metadata> container_type;
        typedef container_type::value_type value_type;
        typedef container_type::iterator iterator;
        typedef container_type::const_iterator const_iterator;
        typedef container_type::size_type size_type;

        metacache_T(const std::string &portdir);

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

    private:
        void reserve(size_type size);
        void push_back(const value_type &);

        std::string _portdir;
        std::vector<std::string> _overlays;
        container_type _metadatas;
};

inline metacache_T::iterator
metacache_T::begin()
{
    return _metadatas.begin();
}

inline metacache_T::const_iterator
metacache_T::begin() const
{
    return _metadatas.begin();
}

inline metacache_T::iterator
metacache_T::end()
{
    return _metadatas.end();
}

inline metacache_T::const_iterator
metacache_T::end() const
{
    return _metadatas.end();
}

inline metacache_T::size_type
metacache_T::size() const
{
    return _metadatas.size();
}

inline bool
metacache_T::empty() const
{
    return _metadatas.empty();
}

inline void
metacache_T::reserve(size_type size)
{
    _metadatas.reserve(size);
}

inline void
metacache_T::push_back(const value_type &v)
{
    _metadatas.push_back(v);
}

#endif

/* vim: set tw=80 sw=4 et : */
