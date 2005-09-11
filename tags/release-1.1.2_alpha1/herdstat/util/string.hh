/*
 * herdstat -- herdstat/util/string.hh
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

#ifndef HAVE_STRING_HH
#define HAVE_STRING_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string>
#include <vector>
#include <sstream>
#include <cstdarg>
#include <herdstat/exceptions.hh>

namespace util
{
    std::string::value_type tolower(const std::string::value_type c);
    std::string::value_type toupper(const std::string::value_type c);
    bool isdigit(const std::string::value_type c);

    /** Convert std::string to all lowercase.
     * @param s String object.
     * @returns Resulting std::string object.
     */
    std::string lowercase(const std::string &s);

    /** Tidy whitespace of the given std::string.
     * @param s String object
     * @returns Resulting std::string object.
     */
    std::string tidy_whitespace(const std::string &s);

    /// sprintf() wrapper
    std::string sprintf(const char *, ...);
    std::string sprintf(const char *, va_list);

    /** Split std::string.
     * @param s String to split.
     * @param d Delimiter.
     * @returns Vector of sub-strings.
     */
    std::vector<std::string> split(const std::string &s,
                              const std::string::value_type d = ' ',
                              bool append_only = false);

    /** Convert vector of std::strings to one std::string.
     * @param v Vector of std::strings
     * @param d Delimiter.
     * @returns Resulting std::string object.
     */
    std::string join(const std::vector<std::string> &v,
                     const std::string::value_type d = ' ');

    /** Convert a type to a std::string.
     * @param v Value of type T.
     * @returns A std::string object.
     */
    template <typename T> std::string stringify(const T &v);

    /** Convert a std::string to a type.
     * @param s A std::string object.
     * @returns Value of type T.
     */
    template <typename T> T destringify(const std::string &s);

    // destringify specializations
    template <> int destringify<int>(const std::string &s);
    template <> long destringify<long>(const std::string &s);
    template <> unsigned long destringify<unsigned long>(const std::string &s);
    template <> double destringify<double>(const std::string &s);
    template <> float destringify<float>(const std::string &s);
    template <> bool destringify<bool>(const std::string &s);

    /** Replace any unfriendly characters in the given std::string to their
     * HTML counterparts.
     * @param s String object.
     * @returns Resulting std::string object.
     */
    std::string htmlify(const std::string &s);

    /** Replace any HTML'ized characters in the given std::string to their
     * respective counterparts.
     * @param s String object.
     * @returns Resulting std::string object.
     */
    std::string unhtmlify(const std::string &s);

    /************************************************************************/
    template <typename T>
    std::string
    stringify(const T &v)
    {
        std::ostringstream os;
        os << v;
        return os.str();
    }
    /************************************************************************/
    template <typename T>
    T
    destringify(const std::string &s)
    {
        std::istringstream is(s.c_str());

        T v;
        is >> v;

        if (not is.eof())
            throw BadCast("Failed to cast '"+s+"'.");

        return v;
    }
    /************************************************************************/
}

#endif

/* vim: set tw=80 sw=4 et : */
