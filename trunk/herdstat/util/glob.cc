/*
 * herdstat -- herdstat/util/glob.cc
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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <herdstat/util/glob.hh>

namespace util {
/****************************************************************************/
glob_T::glob_T(const char *pattern)
{
    int ret = glob(pattern, GLOB_ERR, NULL, &(this->_glob));
    if (ret != 0 and ret != GLOB_NOMATCH)
        throw ErrnoException("glob");

    /* fill results */
    if (ret != GLOB_NOMATCH)
    {
        for (std::size_t i = 0 ; this->_glob.gl_pathv[i] != NULL ; ++i)
            this->_results.push_back(this->_glob.gl_pathv[i]);
    }
}
/****************************************************************************/
glob_T::~glob_T()
{
    globfree(&(this->_glob));
}
/****************************************************************************/
} // namespace util

/* vim: set tw=80 sw=4 et : */
