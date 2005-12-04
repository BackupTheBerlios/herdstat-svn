/*
 * herdstat -- src/cache.hh
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

#ifndef _HAVE_SRC_CACHE_HH
#define _HAVE_SRC_CACHE_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "options.hh"

class Cache
{
    public:
        virtual bool is_valid() const = 0;
        virtual void fill() = 0;
        virtual void load() = 0;
        virtual void dump() = 0;

    protected:
        Cache() { }
        virtual ~Cache() { }
};

class herdstat::io::BinaryIStream;
class herdstat::io::BinaryOStream;

class CacheHeader
{
    public:
        virtual ~CacheHeader() { }
        virtual bool is_valid(herdstat::io::BinaryIStream& stream) const = 0;
        virtual void dump(herdstat::io::BinaryOStream& stream, std::size_t size) = 0;

        const std::size_t& size() const { return _size; }

    protected:
        void set_size(const std::size_t& size) const { _size = size; }

    private:
        mutable std::size_t _size;
};

class PortageCacheHeader : public CacheHeader
{
    public:
        PortageCacheHeader() : _options(GlobalOptions()) { }
        virtual ~PortageCacheHeader() { }

        virtual bool is_valid(herdstat::io::BinaryIStream& stream) const;
        virtual void dump(herdstat::io::BinaryOStream& stream, std::size_t size);

    private:
        const Options& _options;
};

#endif /* _HAVE_SRC_CACHE_HH */

/* vim: set tw=80 sw=4 fdm=marker et : */
