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

#include <vector>
#include <herdstat/cachable.hh>
#include <herdstat/portage/metadata.hh>

/*
 * A cache of all metadata.xml's.
 */

class metacache : public herdstat::cachable
{
    public:
        typedef std::vector<herdstat::portage::metadata> container_type;
        typedef container_type::value_type value_type;
        typedef container_type::iterator iterator;
        typedef container_type::const_iterator const_iterator;
        typedef container_type::size_type size_type;

        metacache(const std::string& portdir);
        ~metacache();

        virtual bool valid() const;
        virtual void fill();
        virtual void load();
        virtual void dump();

        inline const_iterator begin() const;
        inline const_iterator end() const;
        inline size_type size() const;
        inline bool empty() const;

    private:
        Options& _options;
        const std::string& _portdir;
        const std::vector<std::string>& _overlays;
        container_type _metadatas;
};

inline metacache::const_iterator
metacache::begin() const
{
    return _metadatas.begin();
}

inline metacache::const_iterator
metacache::end() const
{
    return _metadatas.end();
}

inline metacache::size_type
metacache::size() const
{
    return _metadatas.size();
}

inline bool
metacache::empty() const
{
    return _metadatas.empty();
}

#endif

/* vim: set tw=80 sw=4 fdm=marker et : */
