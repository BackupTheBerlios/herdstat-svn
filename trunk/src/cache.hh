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

#include <herdstat/noncopyable.hh>
#include <herdstat/io/binary_stream.hh>
#include "common.hh"

class Cache : private herdstat::Noncopyable
{
    public:
        virtual ~Cache() throw() { }

        bool is_valid();
        void fill();
        void load();
        void dump();

        inline const std::string& path() const { return _path; }

    protected:
        Cache(const std::string& path)
            : _options(GlobalOptions()), _path(path), _header(), _stream() { }

        virtual std::size_t cache_size() const = 0;
        virtual const char * const name() const = 0;
        virtual bool do_is_valid() = 0;
        virtual void do_fill() = 0;
        virtual void do_load(herdstat::io::BinaryIStream& stream) = 0;
        virtual void do_dump(herdstat::io::BinaryOStream& stream) = 0;

        inline const std::size_t& header_size() const { return _header.size(); }

        const Options& _options;

    private:
        class Header
        {
            public:
                Header() : _options(GlobalOptions()), _size(0) { }

                bool is_valid(herdstat::io::BinaryIStream& stream);
                void dump(herdstat::io::BinaryOStream& stream,
                          std::size_t size);
                const std::size_t& size() const { return _size; }

            private:
                const Options& _options;
                std::size_t _size;
        };

        std::string _path;
        Header _header;
        herdstat::io::BinaryIStream _stream;
};

#endif /* _HAVE_SRC_CACHE_HH */

/* vim: set tw=80 sw=4 fdm=marker et : */
