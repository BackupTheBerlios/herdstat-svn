/*
 * herdstat -- lib/util_exceptions.hh
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

#ifndef HAVE_UTIL_EXCEPTIONS_HH
#define HAVE_UTIL_EXCEPTIONS_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <exception>
#include <string>
#include <cstdlib>
#include <cstring>
#include <cerrno>

namespace util
{
    class base_E                                : public std::exception { };
    class msg_base_E                            : public base_E
    {
        protected:
            const char *str;

        public:
            msg_base_E() { }
            msg_base_E(const char *msg) : str(msg) { }
            msg_base_E(const std::string &msg) : str(msg.c_str()) { }
            virtual const char *what() const throw() { return str; }
    };

    class errno_E                               : public msg_base_E
    {
        public:
            errno_E() { }
            errno_E(const char *msg) : msg_base_E(msg) { }
            errno_E(const std::string &msg) : msg_base_E(msg) { }
            virtual const char *what() const throw()
            {
                std::string s(str);
                if (s.empty())
                    return std::strerror(errno);
                return (s + ": " + std::strerror(errno)).c_str();
            }
    };

    class bad_fileobject_E                      : public errno_E
    {
        public:
            bad_fileobject_E() { }
            bad_fileobject_E(const char *msg) : errno_E(msg) { }
            bad_fileobject_E(const std::string &msg) : errno_E(msg) { }
    };
}

#endif

/* vim: set tw=80 sw=4 et : */
