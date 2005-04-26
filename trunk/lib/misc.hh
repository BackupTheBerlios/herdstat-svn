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

#include <string>
#include <map>

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
    void debug(const char *, ...);
    bool md5check(const std::string &, const std::string &);
    std::string getcwd();
    std::string get_user_from_email(const std::string &);
    std::string current_user();
    std::string::size_type getcols();
    int fetch(const std::string &, const std::string &, bool);
    int fetch(const char *, const char *, bool);

    class color_map_T
    {
        private:
            class cmap_T : public std::map<color_name_T, std::string>
            {
                public:
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

            static cmap_T _cm;

        public:
	    std::string &operator[](color_name_T c) { return this->_cm[c]; }
    };
}

#endif

/* vim: set tw=80 sw=4 et : */
