/*
 * herdstat -- src/metadata_cache.hh
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

#ifndef HAVE_METADATA_CACHE_HH
#define HAVE_METADATA_CACHE_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <vector>
#include <herdstat/util/progress/meter.hh>
#include <herdstat/portage/metadata.hh>

#include "cache.hh"

/*
 * A cache of all metadata.xml's.
 */

class MetadataCache : public Cache
{
    public:
        typedef std::vector<herdstat::portage::Metadata> container_type;
        typedef container_type::value_type value_type;
        typedef container_type::iterator iterator;
        typedef container_type::const_iterator const_iterator;
        typedef container_type::size_type size_type;

        MetadataCache();
        virtual ~MetadataCache() throw();

        inline const_iterator begin() const;
        inline const_iterator end() const;
        inline size_type size() const;
        inline bool empty() const;

        inline void set_spinner(herdstat::util::ProgressMeter *spinner)
        { _spinner = spinner; }

    protected:
        virtual std::size_t cache_size() const;
        virtual const char * const name() const;
        virtual bool do_is_valid();
        virtual void do_fill();
        virtual void do_load(herdstat::io::BinaryIStream& stream);
        virtual void do_dump(herdstat::io::BinaryOStream& stream);

    private:
        herdstat::util::ProgressMeter *_spinner;
        const std::string& _portdir;
        const std::vector<std::string>& _overlays;
        container_type _metadatas;
};

inline MetadataCache::const_iterator
MetadataCache::begin() const
{
    return _metadatas.begin();
}

inline MetadataCache::const_iterator
MetadataCache::end() const
{
    return _metadatas.end();
}

inline MetadataCache::size_type
MetadataCache::size() const
{
    return _metadatas.size();
}

inline bool
MetadataCache::empty() const
{
    return _metadatas.empty();
}

#endif /* HAVE_METADATA_CACHE_HH */

/* vim: set tw=80 sw=4 fdm=marker et : */
