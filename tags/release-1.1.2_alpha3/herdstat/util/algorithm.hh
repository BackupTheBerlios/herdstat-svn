/*
 * herdstat -- herdstat/util/algorithm.hh
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

#ifndef _HAVE_UTIL_ALGORITHM_HH
#define _HAVE_UTIL_ALGORITHM_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/**
 * @file algorithm.hh
 * @brief Defines the general purpose algorithms.
 */

namespace util {

    /**
     * tranform_if
     * Like std::transform, but only "transforms" if UnaryPred
     * returns true.
     */

    template <typename InputIterator, typename OutputIterator,
              typename UnaryOp, typename UnaryPred>
    OutputIterator
    transform_if(InputIterator first, InputIterator last,
                 OutputIterator result, UnaryPred pred, UnaryOp op)
    {
        while (first != last)
        {
            if (pred(*first))
                *result++ = op(*first);
            ++first;
        }

        return result;
    }

    /**
     * copy_if
     * The infamous non-existent copy_if.
     * Like std::copy but only copies if UnaryPred returns true.
     */

    template <typename InputIterator, typename OutputIterator,
              typename UnaryPred>
    OutputIterator
    copy_if(InputIterator first, InputIterator last,
            OutputIterator result, UnaryPred pred)
    {
        while (first != last)
        {
            if (pred(*first))
                *result++ = *first;
            ++first;
        }

        return result;
    }

} // namespace util

#endif /* _HAVE_UTIL_ALGORITHM_HH */

/* vim: set tw=80 sw=4 et : */
