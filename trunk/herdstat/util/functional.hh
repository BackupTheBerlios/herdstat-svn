/*
 * herdstat -- herdstat/util/functional.hh
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

#ifndef _HAVE_UTIL_FUNCTIONAL_HH
#define _HAVE_UTIL_FUNCTIONAL_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/**
 * @file functional.hh
 * @brief General purpose utility functors.
 */

#include <functional>

namespace util {

    /**
     * Function object that dereferences the given pointers and
     * then calls operator<.
     */

    template <typename T>
    struct DereferenceLess
        : std::binary_function<const T*, const T*, bool>
    {
        bool operator() (const T *p1, const T *p2) const { return (*p1 < *p2); }
    };

    /**
     * Function object that dereferences the given pointers and
     * then calls operator<.
     */

    template <typename T>
    struct DereferenceGreater
        : std::binary_function<const T*, const T*, bool>
    {
        bool operator() (const T *p1, const T *p2) const { return (*p2 < *p1); }
    };

    /**
     * Function object that dereferences the given pointers and
     * then calls operator==.
     */

    template <typename T>
    struct DereferenceEqual
        : std::binary_function<const T*, const T*, bool>
    {
        bool operator() (const T *p1, const T *p2) const { return (*p1 == *p2); }
    };

    /**
     * Function object that dereferences the given pointer and
     * calls operator< against the given string.  Assumes the
     * pointer type defines operator<(std::string).
     */

    template <typename T>
    struct DereferenceStrLess
        : std::binary_function<const T*, std::string, bool>
    {
        bool operator()(const T *p, std::string s) const { return (*p < s); }
    };

    /**
     * Function object that dereferences the given pointer and
     * calls operator> against the given string.  Assumes the
     * pointer type defines operator>(std::string).
     */

    template <typename T>
    struct DereferenceStrGreater
        : std::binary_function<const T*, std::string, bool>
    {
        bool operator()(const T *p, std::string s) const { return (*p > s); }
    };

    /**
     * Function object that dereferences the given pointer and
     * calls operator== against the given string.  Assumes the
     * pointer type defines operator==(std::string).
     */

    template <typename T>
    struct DereferenceStrEqual
        : std::binary_function<const T*, std::string, bool>
    {
        bool operator()(const T *p, std::string s) const { return (*p == s); }
    };

    /**
     * Function object that deletes the given pointer and sets
     * it to NULL.
     */

    struct DeleteAndNullify
    {
        template <typename T>
        void operator()(T *p) const
        {
            if (p)
            {
                delete p;
                p = NULL;
            }
        }
    };

} // namespace util

#endif /* _HAVE_UTIL_FUNCTIONAL_HH */

/* vim: set tw=80 sw=4 et : */
