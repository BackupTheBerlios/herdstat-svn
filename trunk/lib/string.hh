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

#ifdef UNICODE
# include <glib/gtypes.h>
# include <glibmm/ustring.h>
#else /* UNICODE */
# include <string>
#endif /* UNICODE */

namespace util
{
#ifdef UNICODE
    class string : public Glib::ustring
    {
        public:
            explicit string() : Glib::ustring() { }
            string(const char *n) : Glib::ustring(n) { }
            string(const std::string &n) : Glib::ustring(n) { }
            string(const Glib::ustring &n) : Glib::ustring(n) { }
            string(const string &n) : Glib::ustring(n) { }
#else /* UNICODE */
    class string : public std::string
    {
        public:
            explicit string() : std::string() { }
            string(const char *n) : std::string(n) { }
            string(const std::string &n) : std::string(n) { }
            string(const string &n) : std::string(n) { }
#endif /* UNICODE */
            virtual ~string() { }

            virtual std::vector<util::string>
            split(const util::string::value_type delim = ' ') const;
    };

    util::string lowercase(const util::string &);
    util::string tidy_whitespace(const util::string &);

#ifdef UNICODE
    util::string sprintf(const gchar *, ...);
    util::string sprintf(const gchar *, va_list);
#else /* UNICODE */
    util::string sprintf(const char *, ...);
    util::string sprintf(const char *, va_list);
#endif /* UNICODE */

    std::vector<util::string> split(const util::string &,
                                    const util::string::value_type d = ' ');
    util::string stringify(const std::vector<util::string> &,
                           const util::string::value_type d = ' ');
}

#endif

/* vim: set tw=80 sw=4 et : */
