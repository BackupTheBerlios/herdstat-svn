/*
 * herdstat -- src/exceptions.hh
 * $Id$
 * Copyright (c) 2005 Aaron Walker <ka0ttic at gentoo.org>
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

#ifndef HAVE_EXCEPTIONS_HH
#define HAVE_EXCEPTIONS_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <exception>
#include <stdexcept>

#include "common.hh"

/* base exceptions */
class herdstat_base_E                   : public std::exception { };
class herdstat_base_bad_cast_E          : public std::bad_cast,
                                                 herdstat_base_E  { };

class herdstat_msg_base_E               : public herdstat_base_E
{
    protected:
        const char *str;

    public:
        herdstat_msg_base_E() { }
        herdstat_msg_base_E(const char *msg) : str(msg) { }
        herdstat_msg_base_E(const util::string &msg) : str(msg.c_str()) { }
        virtual const char *what() const throw() { return str; }
};

class herdstat_va_base_E                : public herdstat_msg_base_E
{
    protected:
        const char *str;
        va_list v;

    public:
        herdstat_va_base_E() { }
        herdstat_va_base_E(const char *msg, va_list v)
            : str(util::sprintf(msg, v).c_str()) { }
        herdstat_va_base_E(const util::string &msg, va_list v)
            : str(util::sprintf(msg.c_str(), v).c_str()) { }

        herdstat_va_base_E(const char *msg, ...)
        {
            va_start(v, msg);
            str = util::sprintf(msg, v).c_str();
            va_end(v);
        }

        herdstat_va_base_E(const util::string &msg, ...)
        {
            va_start(v, msg.c_str());
            str = util::sprintf(msg.c_str(), v).c_str();
            va_end(v);
        }

        virtual const char *what() const throw() { return str; }
};

#ifdef UNICODE
class herdstat_glib_E                   : public Glib::Error { };
#endif

/* command line handling exceptions */
class args_E                            : public herdstat_base_E { };
class args_usage_E                      : public args_E { };
class args_one_action_only_E            : public args_usage_E { };
class args_help_E                       : public args_E { };
class args_version_E                    : public args_E { };
class args_unimplemented_E              : public args_E { };

class bad_option_cast_E                 : public herdstat_base_bad_cast_E { };

class invalid_option_E                  : public herdstat_msg_base_E
{
    public:
        invalid_option_E(util::string const &msg) : herdstat_msg_base_E(msg) {}
        virtual const char *what() const throw()
        {
            return util::sprintf("Invalid option '%s'", str).c_str();
        }
};

class errno_error_E                     : public herdstat_msg_base_E
{
    public:
        errno_error_E() { }
        errno_error_E(const char *msg) : herdstat_msg_base_E(msg) { }
        errno_error_E(const util::string &msg) : herdstat_msg_base_E(msg) { }
        virtual const char *what() const throw()
        {
            util::string s(str);
            if (s.empty())
                return std::strerror(errno);
            return (s + ": " + std::strerror(errno)).c_str();
        }
};

class bad_fileobject_E                  : public errno_error_E
{
    public:
        bad_fileobject_E() { }
        bad_fileobject_E(const char *msg) : errno_error_E(msg) { }
        bad_fileobject_E(const util::string &msg) : errno_error_E(msg) { }
};

class bad_herdsXML_E                    : public herdstat_base_E { };

class timer_E                           : public herdstat_base_E
{
    private:
        long _ms;

    public:
        timer_E(long ms) : herdstat_base_E(), _ms(ms) { }
        virtual const char *what() const throw()
        {
            return util::sprintf("%ld", _ms).c_str();
        }
};

/* action handler exceptions */
class action_E                          : public herdstat_base_E { };
class herd_E                            : public action_E { };
class dev_E                             : public action_E { };
class fetch_E                           : public action_E { };

/* formatter exceptions */
class format_E                          : public herdstat_va_base_E
{

    public:
        format_E() { }
        format_E(const char *msg, va_list v) : herdstat_va_base_E(msg, v) { }
        format_E(const char *msg, ...)
        {
            va_start(v, msg);
            str = util::sprintf(msg, v).c_str();
            va_end(v);
        }
};

#endif

/* vim: set tw=80 sw=4 et : */
