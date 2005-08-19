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

#include "util_exceptions.hh"

namespace util
{
    /**
     * POSIX glob() interface.  Fills container with all files
     * matching a glob pattern.
     */

    class glob : private noncopyable
    {
        public:
            typedef std::string value_type;
            typedef std::vector<value_type>::iterator iterator;
            typedef std::vector<value_type>::const_iterator const_iterator;
            typedef std::vector<value_type>::reverse_iterator reverse_iterator;
            typedef std::vector<value_type>::const_reverse_iterator const_reverse_iterator;
            typedef std::vector<value_type>::size_type size_type;

            /** Constructor.
             * @param pattern Glob pattern.
             */
            glob(const char *pattern) : _glob(), _results()
            {
                int rv = ::glob(pattern, GLOB_ERR, NULL, &(this->_glob));
                if (rv != 0 and rv != GLOB_NOMATCH)
                    throw util::errno_E("glob");

                /* fill vector with glob results */
                for (std::size_t i = 0 ; this->_glob.gl_pathv[i] ; ++i)
                    this->_results.push_back(this->_glob.gl_pathv[i]);
            }

            /// Destructor.
            ~glob() { globfree(&(this->_glob)); }

            iterator begin() { return _results.begin(); }
            const_iterator begin() const { return _results.begin(); }
            iterator end() { return _results.end(); }
            const_iterator end() const { return _results.end(); }
            reverse_iterator rbegin() { return _results.rbegin(); }
            const_reverse_iterator rbegin() const { return _results.rbegin(); }
            reverse_iterator rend() { return _results.rend(); }
            const_reverse_iterator rend() const { return _results.rend(); }

            size_type size() const { return _results.size(); }
            bool empty() const { return this->_results.size() != 0; }

        private:
            /// Internal glob_t instance.
            glob_t _glob;
            /// Glob results container.
            std::vector<std::string> _results;
    };

    /**
     * fnmatch() functor interface.
     */

    class patternMatch : private noncopyable
    {
        public:
            /** Overloaded operator().
             * @param pattern Glob pattern.
             * @param path Path.
             * @returns A boolean value (Does glob match pattern?).
             */
            bool operator() (const std::string pattern,
                             const std::string path)
            { return (*this)(pattern.c_str(), path.c_str()); }

            /** fnmatch() wrapper.
             * @param pattern Glob pattern.
             * @param path Path.
             * @returns A boolean value (Does glob match pattern?).
             */
            bool operator() (const char *pattern, const char *path)
            {
                return fnmatch(pattern, path, 0) == 0;
            }
    };
}

#endif

/* vim: set tw=80 sw=4 et : */
