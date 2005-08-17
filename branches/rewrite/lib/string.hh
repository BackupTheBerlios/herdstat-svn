/*
 * herdstat -- lib/string.hh
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

#include <vector>
#include <sstream>
#include <cstdarg>

#ifdef UNICODE
# include <glib/gtypes.h>
# include <glibmm/ustring.h>
#else /* UNICODE */
# include <string>
#endif /* UNICODE */

namespace util
{
    /** string derivative.
     * Derives from Glib::ustring if --enable-unicode.
     * Otherwise derives from std::string.
     */

#ifdef UNICODE

    class string : public Glib::ustring
    {
        public:
            typedef Glib::ustring base_type;
            string(const std::string &n) : base_type(n) { }

#else /* UNICODE */

    class string : public std::string
    {
        public:
            typedef std::string base_type;

#endif /* UNICODE */
            explicit string() : base_type() { }
            string(const char *n) : base_type(n) { }
            string(const base_type &n) : base_type(n) { }
            string(const string &n) : base_type(n) { }

            virtual ~string() { }

            string substr(size_type i = 0, size_type n = npos) const
            { return base_type::substr(i, n); }

            /** Split string.
             * @param delim  Delimiter.
             * @param append Append empty string if delim not found?
             *               (defaults to false).
             * @returns      A vector of sub-strings.
             */
            virtual std::vector<std::string>
            split(const value_type delim = ' ', bool append = false) const;
    };

    string::value_type tolower(const string::value_type c);
    string::value_type toupper(const string::value_type c);
    bool isdigit(const string::value_type c);

    /** Convert string to all lowercase.
     * @param s String object.
     * @returns Resulting string object.
     */
    string lowercase(const string &s);

    /** Tidy whitespace of the given string.
     * @param s String object
     * @returns Resulting string object.
     */
    string tidy_whitespace(const string &s);

    /// sprintf() wrapper
#ifdef UNICODE
    std::string sprintf(const gchar *, ...);
    std::string sprintf(const gchar *, va_list);
#else /* UNICODE */
    std::string sprintf(const char *, ...);
    std::string sprintf(const char *, va_list);
#endif /* UNICODE */

    /** Split string.
     * @param s String to split.
     * @param d Delimiter.
     * @returns Vector of sub-strings.
     */
    std::vector<string> split(const string &s,
                              const string::value_type d = ' ',
                              bool append_only = false);

    /** Convert vector of strings to one string.
     * @param v Vector of strings
     * @param d Delimiter.
     * @returns Resulting string object.
     */
    string join(const std::vector<string> &v, const string::value_type d = ' ');

    /** Convert a type to a string.
     * @param v Value of type T.
     * @returns A string object.
     */
    template <typename T> string stringify(const T &v);

    /** Convert a string to a type.
     * @param s A string object.
     * @returns Value of type T.
     */
    template <typename T> T destringify(const string &s);

    // destringify specializations
    template <> int destringify<int>(const string &s);
    template <> long destringify<long>(const string &s);
    template <> unsigned long destringify<unsigned long>(const string &s);
    template <> double destringify<double>(const string &s);
    template <> float destringify<float>(const string &s);
    template <> bool destringify<bool>(const string &s);

    /** Replace any unfriendly characters in the given string to their
     * HTML counterparts.
     * @param s String object.
     * @returns Resulting string object.
     */
    string htmlify(const string &s);

    /** Replace any HTML'ized characters in the given string to their
     * respective counterparts.
     * @param s String object.
     * @returns Resulting string object.
     */
    string unhtmlify(const string &s);

    /************************************************************************/

    template <typename T>
    inline string
    stringify(const T &v)
    {
        std::ostringstream os;
        os << v;
        return os.str();
    }
}

#endif

/* vim: set tw=80 sw=4 et : */
