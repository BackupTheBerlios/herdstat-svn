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
#include "exceptions.hh"

/*
 * Represents a cache of objects of type T.
 */

template <typename T>
class cache_T
{
    protected:
        const std::string _file;
        std::vector<T> _cache;

    public:
        typedef typename std::vector<T>::iterator iterator;
        typedef typename std::vector<T>::size_type size_type;

        cache_T(const std::string &f) : _file(f) { }
        virtual ~cache_T() { }

        iterator begin() { return _cache.begin(); }
        iterator end() { return _cache.end(); }
        size_type size() const { return _cache.size(); }

        void read()
        {
            std::auto_ptr<std::ifstream> f(new std::ifstream(_file.c_str()));
            if (not (*f))
                throw bad_fileobject_E(_file);

            std::copy(std::istream_iterator<T>(*f),
                std::istream_iterator<T>(), std::back_inserter(_cache));
        }

        void write()
        {
            std::auto_ptr<std::ofstream> f(new std::ofstream(_file.c_str()));
            if (not (*f))
                throw bad_fileobject_E(_file);
 
            std::copy(_cache.begin(), _cache.end(),
                std::ostream_iterator<T>(*f, "\n"));
        }

        virtual bool valid() = 0;
        virtual void fill()  = 0;
};

#endif

/* vim: set tw=80 sw=4 et : */
