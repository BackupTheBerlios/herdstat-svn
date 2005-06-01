/*
 * herdstat -- lib/cache.hh
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

#include <vector>
#include "file.hh"

namespace util
{
    /*
     * Represents a cache of data on disk.  The template argument
     * is the data type the cache will be stored in during run-time.
     */

    template <class C>
    class cache_T
    {
        public:
            typedef util::file_T file_type;
            typedef util::string string_type;
            typedef C value_type;
            typedef typename value_type::iterator iterator;
            typedef typename value_type::const_iterator const_iterator;
            typedef typename value_type::size_type size_type;

            cache_T(const string_type &p) : _path(p) { }

            virtual ~cache_T() { }

            /* vector subset */
            iterator begin() { return this->_cache.begin(); }
            const_iterator begin() const { return this->_cache.begin(); }
            iterator end() { return this->_cache.end(); }
            const_iterator end() const { return this->_cache.end(); }
            size_type size() const { return this->_cache.size(); }
            void clear() { this->_cache.clear(); }
            bool empty() const { return this->_cache.size() == 0; }

            virtual void init()
            {
                if (this->valid())
                    this->load();
                else
                {
                    this->fill();
                    this->dump();
                }
            }

            virtual void fill() { }
            virtual void load() { }

            /* pure virtuals */
            virtual bool valid() const = 0;
            virtual void dump() = 0;

        protected:
            const string_type _path;
            value_type _cache;
    };
}

#endif

/* vim: set tw=80 sw=4 et : */
