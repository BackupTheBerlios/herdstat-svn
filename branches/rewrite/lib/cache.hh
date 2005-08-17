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

/**
 * @file cache.hh
 * @brief Defines the abstrace cache interface.
 */

namespace util
{
    /**
     * Abstract interface for a cache of data.  Template argument
     * is the container type in which the data will be stored.
     */

    template <class T>
    class cache
    {
        public:
            virtual ~cache() { }

            /** Determine whether cache is valid.
             * @returns True if valid.
             */
            virtual bool valid() const = 0;

            /// Fill cache with data.
            virtual void fill() = 0;

            /// Load cache.
            virtual void load() = 0;

            /// Dump cache.
            virtual void dump() = 0;

        protected:
            /// Default logic implementation.
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

        private:
            T container;
    };
}

#endif

/* vim: set tw=80 sw=4 et : */
