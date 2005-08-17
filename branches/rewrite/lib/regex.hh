/*
 * herdstat -- lib/regex.hh
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

#ifndef HAVE_REGEX_HH
#define HAVE_REGEX_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <functional>
#include <cstddef>
#include <cassert>
#include <sys/types.h>
#include <regex.h>
#include "string.hh"

namespace util
{
    /**
     * POSIX Regular Expressions interface.
     */

    class regex
    {
        public:
            typedef std::string string_type;
            typedef regex_t regex_type;

            /// Default constructor.
            regex() : _compiled(false), _cflags(0), _eflags(0) { }

            /** Constructor.
             * @param r regular expression string.
             * @param c CFLAGS.
             * @param e EFLAGS.
             */
            regex(const string_type &r, int c = 0, int e = 0)
                : _str(r), _compiled(false), _cflags(c), _eflags(e)
            { this->assign(r, c, e); }

            ~regex() { if (this->_compiled) this->cleanup(); }

            /** Assign a new regular expression.
             * @param r regular expression string.
             * @param c CFLAGS.
             * @param e EFLAGS.
             */
            void assign(const string_type &r, int c = 0, int e = 0);

            /** Assign a new regular expression (no CFLAGS/EFLAGS).
             * @param s regular expression string.
             * @returns a regex object.
             */
            regex &operator= (const string_type &s)
            { this->assign(s); return *this; }

            /** Determine if this regex matches the specified string.
             * @param cmp Comparison string.
             * @returns   A boolean value.
             */
            bool operator== (const string_type &cmp) const;

            /** Determine if this regex does not match the specified string.
             * @param cmp Comparison string.
             * @returns   A boolean value.
             */
            bool operator!= (const string_type &cmp) const
            { return not (*this == cmp); }

            /** Get regular expression string.
             * @returns A string object.
             */
            const string_type &operator()() const { return this->_str; }

            /** Is this regular expression string empty?
             * @returns A boolean value.
             */
            bool empty() const { return this->_str.empty(); }

        private:
            /// Clean up compiled regex_t
            void cleanup();

            /// regular expression string
            string_type _str;
            /// has our regex_t been compiled?
            bool        _compiled;
            /// cflags
            int         _cflags;
            /// eflags
            int         _eflags;
            /// regex_t
            regex_type  _regex;
    };

    /**
     * Functor for using regex with standard algorithms.
     */

    class regexMatch : public std::binary_function<regex *, string, bool>
    {
        public:
            /** Does regex match string?
             * @param re Pointer to a regex object.
             * @param s  String object.
             * @returns  A boolean value.
             */
            bool operator() (regex *re, string s) const
            { return (*re == s); }
    };
}

#endif

/* vim: set tw=80 sw=4 et : */
