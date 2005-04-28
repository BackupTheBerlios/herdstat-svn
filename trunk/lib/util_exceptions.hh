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
#include <sys/types.h>
#include <regex.h>

#include "string.hh"

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
            msg_base_E(const util::string &msg) : str(msg.c_str()) { }
            virtual const char *what() const throw() { return this->str; }
    };

    class errno_E                               : public msg_base_E
    {
        public:
            errno_E() { }
            errno_E(const char *msg) : msg_base_E(msg) { }
            errno_E(const util::string &msg) : msg_base_E(msg) { }
            virtual const char *what() const throw()
            {
                util::string s(this->str);
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
            bad_fileobject_E(const util::string &msg) : errno_E(msg) { }
    };

    class bad_regex_E                           : public base_E
    {
        private:
            const char *_str;
            int _err;
            regex_t *_re;

        public:
            bad_regex_E() : _str(NULL), _err(0), _re(NULL) { }
            bad_regex_E(int e, regex_t *re) : _str(NULL), _err(e), _re(re) { }
            bad_regex_E(const util::string &s) : _str(s.c_str()), _err(0),
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
		    util::string s(buf);
		    std::free(buf);
		    return s.c_str();
	        }

	        return this->_str ? this->_str : "";
            }
    };
}

#endif

/* vim: set tw=80 sw=4 et : */
