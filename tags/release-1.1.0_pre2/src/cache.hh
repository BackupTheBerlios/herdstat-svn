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

#ifndef HAVE_CACHE_HH
#define HAVE_CACHE_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <fstream>
#include <vector>
#include <string>
#include <iterator>
#include <algorithm>

#include "common.hh"

/*
 * Represents a cache of objects of type T.
 */

class cache_T
{
    protected:
        const std::string _file;
        std::vector<std::string> _cache;

    public:
        typedef std::vector<std::string>::iterator iterator;
        typedef std::vector<std::string>::size_type size_type;

        cache_T(const std::string &f, size_type reserve = 0) : _file(f)
        {
            if (reserve != 0)
                _cache.reserve(reserve);
        }

        virtual ~cache_T() { }

        /* vector subset */
        iterator begin() { return _cache.begin(); }
        iterator end() { return _cache.end(); }
        size_type size() const { return _cache.size(); }
        void clear() { _cache.clear(); }

        virtual void init()
        {
            if (this->valid())
                this->read();
            else
            {
                this->fill();
                this->write();
            }
        }

        /* read cache from disk */
        void read()
        {
            util::file_T f(_file);
            f.open();
            f.read(&_cache);
        }

        /* write cache to disk */
        void write() const
        {
            util::file_T f(_file);
            f.open(std::ios::out);
            f.write(_cache);
        }

        /* pure virtuals */
        virtual bool valid() const = 0;
        virtual void fill() = 0;
};

#endif

/* vim: set tw=80 sw=4 et : */
