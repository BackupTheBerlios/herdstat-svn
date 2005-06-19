/*
 * herdstat -- lib/glob.hh
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

#ifndef HAVE_GLOB_HH
#define HAVE_GLOB_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <vector>

#ifdef HAVE_GLOB_H
# include <glob.h>
#endif /* HAVE_GLOB_H */
#ifdef HAVE_FNMATCH_H
# include <fnmatch.h>
#endif /* HAVE_FNMATCH_H */

#include "file.hh"
#include "util_exceptions.hh"

namespace util
{
    /*
     * glob() interface
     */

    class glob_T : public std::vector<util::path_T>
    {
        public:
            glob_T(const char *pattern)
            {
                int rv = glob(pattern, GLOB_ERR, NULL, &(this->_glob));
                if (rv != 0 and rv != GLOB_NOMATCH)
                    throw util::errno_E("glob");

                /* fill vector with glob results */
                for (std::size_t i = 0 ; this->_glob.gl_pathv[i] ; ++i)
                    this->push_back(this->_glob.gl_pathv[i]);
            }

            ~glob_T() { globfree(&(this->_glob)); }

        protected:
            glob_t _glob;
    };

    /*
     * fnmatch() functor interface.
     */

    class patternMatch
    {
        public:
            bool operator() (const util::string pattern,
                             const util::string path)
            { return (*this)(pattern.c_str(), path.c_str()); }

            bool operator() (const char *pattern, const char *path)
            {
                return fnmatch(pattern, path, 0) == 0;
            }
    };
}

#endif

/* vim: set tw=80 sw=4 et : */
