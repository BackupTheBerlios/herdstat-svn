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
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <cstdarg>
#include <sys/types.h>
#include <regex.h>

#include "string.hh"

namespace util
{
    /// base exception
    class base_E                                : public std::exception { };

    /// base message exception
    class msg_base_E                            : public base_E
    {
        protected:
            const char *str;

        public:
            msg_base_E() { }
            msg_base_E(const char *msg) : str(msg) { }
            msg_base_E(const string &msg) : str(msg.c_str()) { }
            virtual const char *what() const throw() { return this->str; }
    };

    /// base variable message exception
    class va_msg_base_E                         : public msg_base_E
    {
        protected:
            const char *str;
            va_list v;

        public:
            va_msg_base_E() { }
            va_msg_base_E(const char *msg, va_list v)
                : str(util::sprintf(msg, v).c_str()) { }
            va_msg_base_E(const string &msg, va_list v)
                : str(util::sprintf(msg.c_str(), v).c_str()) { }
            va_msg_base_E(const char *msg, ...)
            {
                va_start(v, msg);
                str = util::sprintf(msg, v).c_str();
                va_end(v);
            }
            va_msg_base_E(const string &msg, ...)
            {
#ifdef HAVE_GCC4
                va_start(v, msg);
#else
                va_start(v, msg.c_str());
#endif /* HAVE_GCC4 */
                str = util::sprintf(msg.c_str(), v).c_str();
                va_end(v);
            }
    };

    /// strerror() exception
    class errno_E                               : public msg_base_E
    {
        public:
            errno_E() { }
            errno_E(const char *msg) : msg_base_E(msg) { }
            errno_E(const string &msg) : msg_base_E(msg) { }
            virtual const char *what() const throw()
            {
                string s(this->str);
                if (s.empty())
                    return std::strerror(errno);
                return (s + ": " + std::strerror(errno)).c_str();
            }
    };

    /// bad file object exception
    class bad_fileobject_E                      : public errno_E
    {
        public:
            bad_fileobject_E() { }
            bad_fileobject_E(const char *msg) : errno_E(msg) { }
            bad_fileobject_E(const string &msg) : errno_E(msg) { }
    };

    /// bad regular expression exception
    class bad_regex_E                           : public base_E
    {
        private:
            const char *_str;
            int _err;
            const regex_t *_re;

        public:
            bad_regex_E() : _str(NULL), _err(0), _re(NULL) { }
            bad_regex_E(int e, const regex_t *re)
                : _str(NULL), _err(e), _re(re) { }
            bad_regex_E(const string &s) : _str(s.c_str()), _err(0),
                                                _re(NULL) { }
            virtual const char *what() const throw()
            {
	        if (not this->_str and (not this->_re or this->_err == 0))
		    return "";

	        if (this->_str)
	        {
		    std::size_t len = regerror(this->_err, this->_re, NULL, 0);
		    char *buf = (char *)std::malloc(len);
		    if (not buf)
		        throw msg_base_E("Failed to allocate memory.");

		    regerror(this->_err, this->_re, buf, len);
		    string s(buf);
		    std::free(buf);
		    return s.c_str();
	        }

	        return this->_str ? this->_str : "";
            }
    };

    class bad_cast_E : public msg_base_E
    {
        public:
            bad_cast_E() { }
            bad_cast_E(const char *msg) : msg_base_E(msg) { }
            bad_cast_E(const string &msg) : msg_base_E(msg) { }
    };
}

#endif

/* vim: set tw=80 sw=4 et : */
