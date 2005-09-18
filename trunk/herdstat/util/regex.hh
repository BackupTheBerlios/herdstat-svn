/*
 * herdstat -- herdstat/util/regex.hh
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

#include <string>
#include <functional>
#include <cstddef>
#include <cassert>
#include <sys/types.h>
#include <regex.h>

namespace util
{
    /**
     * POSIX Regular Expressions interface.
     */

    class Regex
    {
        public:
            enum cflags { };
            enum eflags { };

            static const cflags extended;
            static const cflags icase;
            static const cflags nosub;
            static const cflags newline;

            static const eflags notbol;
            static const eflags noteol;

            /// Default constructor.
            Regex();

            /** Constructor.
             * @param r regular expression std::string.
             * @param c CFLAGS.
             * @param e EFLAGS.
             */
            Regex(const std::string& r, int c = 0, int e = 0);

            /// Destructor.
            ~Regex();

            /** Assign a new regular expression.
             * @param r regular expression std::string.
             * @param c CFLAGS.
             * @param e EFLAGS.
             */
            void assign(const std::string& r , int c = 0, int e = 0);

            /** Assign a new regular expression (no CFLAGS/EFLAGS).
             * @param s regular expression std::string.
             * @returns a Regex object.
             */
            Regex &operator= (const std::string& s);

            /** Determine if this regex matches the specified std::string.
             * @param cmp Comparison std::string.
             * @returns   A boolean value.
             */
            bool operator== (const std::string& cmp) const;

            /** Determine if this regex does not match the specified std::string.
             * @param cmp Comparison std::string.
             * @returns   A boolean value.
             */
            bool operator!= (const std::string& cmp) const;

            /** Get regular expression std::string.
             * @returns A std::string object.
             */
            const std::string& operator()() const;

            /** Is this regular expression std::string empty?
             * @returns A boolean value.
             */
            bool empty() const;

        private:
            /// Clean up compiled regex_t
            void cleanup();
            /// Compile regex.
            void compile();

            std::string _str;
            bool        _compiled;
            int         _cflags;
            int         _eflags;
            regex_t     _regex;
    };

    inline Regex&
    Regex::operator= (const std::string& re)
    {
        this->assign(re);
        return *this;
    }

    inline bool
    Regex::operator== (const std::string& cmp) const
    {
        return (regexec(&_regex, cmp.c_str(), 0, NULL, _eflags) == 0);
    }

    inline bool
    Regex::operator!= (const std::string& cmp) const
    {
        return not (*this == cmp);
    }

    inline const std::string&
    Regex::operator()() const
    {
        return _str;
    }

    inline bool
    Regex::empty() const
    {
        return _str.empty();
    }

    /**
     * Functor for using Regex with standard algorithms.
     */

    struct regexMatch : std::binary_function<const Regex *, std::string, bool>
    {
        bool operator() (const Regex *re, const std::string& s) const
        { return (*re == s); }
    };
}

#endif /* HAVE_REGEX_HH */

/* vim: set tw=80 sw=4 et : */
