/*
 * herdstat -- herdstat/util/glob.hh
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

#include <herdstat/exceptions.hh>

namespace util
{
    /**
     * POSIX glob() interface.  Fills container with all files
     * matching a glob pattern.
     */

    class glob_T
    {
        public:
            typedef std::vector<std::string> container_type;
            typedef container_type::const_iterator const_iterator;
            typedef container_type::size_type size_type;

            glob_T(const char *pattern);
            ~glob_T();

            const_iterator begin() const { return _results.begin(); }
            const_iterator end()   const { return _results.end(); }
            size_type size() const { return _results.size(); }
            bool empty() const { return _results.empty(); }

        private:
            glob_t _glob;
            container_type _results;
    };

    /**
     * fnmatch() functor interface.
     */

    class patternMatch
    {
        public:
            /** fnmatch() wrapper.
             * @param pattern Glob pattern.
             * @param path Path.
             * @returns A boolean value (Does glob match pattern?).
             */
            bool operator() (const std::string pattern,
                             const std::string path) const
            { return (fnmatch(pattern.c_str(), path.c_str(), 0) == 0); }
    };
}

#endif

/* vim: set tw=80 sw=4 et : */
