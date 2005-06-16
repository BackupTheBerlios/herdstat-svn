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
    class cache_T
    {
        public:
            typedef util::file_T file_type;
            typedef util::string string_type;
            typedef std::vector<string_type> value_type;
            typedef value_type::iterator iterator;
            typedef value_type::const_iterator const_iterator;
            typedef value_type::size_type size_type;

            cache_T(const string_type &f, size_type reserve = 0) : _file(f)
            {
                if (reserve != 0)
                    this->_cache.reserve(reserve);
            }

            virtual ~cache_T() { }

            /* vector subset */
            iterator begin() { return this->_cache.begin(); }
            const_iterator begin() const { return this->_cache.begin(); }
            iterator end() { return this->_cache.end(); }
            const_iterator end() const { return this->_cache.end(); }
            size_type size() const { return this->_cache.size(); }
            void clear() { this->_cache.clear(); }

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
            virtual void read()
            {
                file_type f(this->_file);
                f.open();
                f.read(&(this->_cache));
            }

            /* write cache to disk */
            virtual void write() const
            {
                file_type f(this->_file);
                f.open(std::ios::out);
                f.write(this->_cache);
            }

            /* pure virtuals */
            virtual bool valid() const = 0;
            virtual void fill() = 0;

        protected:
            const string_type _file;
            value_type _cache;
    };
}

#endif

/* vim: set tw=80 sw=4 et : */
