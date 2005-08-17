/*
 * herdstat -- lib/misc.hh
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

#ifndef HAVE_MISC_HH
#define HAVE_MISC_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <map>
#ifdef UNICODE
# include <glib/gtypes.h>
#endif /* UNICODE */
#include "string.hh"

enum color_name_T
{
    red,
    green,
    blue,
    yellow,
    orange,
    magenta,
    cyan,
    white,
    black,
    none
};

/* commonly-used utility functions */

namespace util
{
#ifdef UNICODE
    void debug(const gchar *, ...);
#else /* UNICODE */
    void debug(const char *, ...);
#endif /* UNICODE */

    /// getcwd() wrapper.
    std::string getcwd();

    /** Determine username from email address.
     * @param e E-mail address.
     * @returns A string object.
     */
    std::string get_user_from_email(const std::string &e);

    /** Determine current user.
     * Uses ECHANGELOG_USER, if set.  Uses USER otherwise.
     * @returns A string object.
     */
    std::string current_user();

    /// Determine current terminal width.
    std::string::size_type getcols();

    /** Format date string.
     * @param t Reference to a time_t object.
     * @param f Format string as accepted by strftime(3).
     * @returns A constant string object.
     */
    const std::string format_date(const std::time_t &t,
        const char *f = "%a %b %d %T %Z %Y");

    /** Format date string.
     * @param t Reference to a string object that contains the time.
     * @param f Format string as accepted by strftime(3).
     * @returns A constant string object.
     */
    const std::string format_date(const std::string &t,
        const char *f = "%a %b %d %T %Z %Y");

    /**
     * Color map.
     */

    class color_map_T
    {
        public:
            typedef std::string string_type;

            /** Access color string associated with the given color type.
             * @param c Color type (color_name_T).
             * @returns The associated string object.
             */
	    string_type &operator[](color_name_T c) { return this->_cm[c]; }

            /** Access color string associated with the given color name.
             * @param s Color name.
             * @returns The associated string object.
             */
            string_type &operator[](const string_type &s);

        private:
            /// Internal color map.
            class cmap_T : public std::map<color_name_T, string_type>
            {
                public:
                    /// Default constructor.  Fills map.
                    cmap_T()
                    {
                        (*this)[red]     = "\033[0;31m";
                        (*this)[green]   = "\033[0;32m";
                        (*this)[blue]    = "\033[1;34m";
                        (*this)[yellow]  = "\033[1;33m";
                        (*this)[orange]  = "\033[0;33m";
                        (*this)[magenta] = "\033[1;35m";
                        (*this)[cyan]    = "\033[1;36m";
                        (*this)[black]   = "\033[0;30m";
                        (*this)[white]   = "\033[0;1m";
                        (*this)[none]    = "\033[00m";
                    }
            };

            /// Internal color map.
            static cmap_T _cm;
    };
}

#endif

/* vim: set tw=80 sw=4 et : */
