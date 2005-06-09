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

#include "string.hh"
#include "file.hh"

namespace util
{
    /*
     * Abstract interface for a cache of data.  Template argument
     * is the container type in which the data will be stored.
     */

    template <class C>
    class cache_T : public C
    {
        public:
            typedef util::string string_type;
            typedef C value_type;

            cache_T(const string_type &p) : _path(p) { }
            virtual ~cache_T() { }

            virtual bool valid() const = 0;
            virtual void fill() = 0;
            virtual void load() = 0;
            virtual void dump() = 0;

        protected:
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

            const string_type _path;
    };
}

#endif

/* vim: set tw=80 sw=4 et : */
