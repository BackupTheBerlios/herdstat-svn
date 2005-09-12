/*
 * herdstat -- herdstat/util/functors.hh
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

#ifndef _HAVE_FUNCTORS_HH
#define _HAVE_FUNCTORS_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/**
 * @file functors.hh
 * @brief General purpose utility functors.
 */

namespace util {

    class DereferenceLess
    {
        public:
            template <typename Type>
            bool operator() (Type p1, Type p2) const { return (*p1 < *p2); }
    };

    class DereferenceGreater
    {
        public:
            template <typename Type>
            bool operator() (Type p1, Type p2) const { return (*p2 < *p1); }
    };

    class DereferenceEqual
    {
        public:
            template <typename Type>
            bool operator() (Type p1, Type p2) const { return (*p1 == *p2); }
    };

} // namespace util

#endif /* _HAVE_FUNCTORS_HH */

/* vim: set tw=80 sw=4 et : */
