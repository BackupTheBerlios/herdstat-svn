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

#include <herdstat/exceptions.hh>

/* command line handling exceptions */
class argsException     : public herdstat::BaseException { };
class argsUsage         : public argsException { };
class argsOneActionOnly : public argsUsage { };
class argsHelp          : public argsException { };
class argsVersion       : public argsException { };
class argsUnimplemented : public argsException { };
class argsInvalidField  : public argsException { };

class InvalidField : public herdstat::Exception
{
    public:
        InvalidField() { }
        InvalidField(const char *msg) : herdstat::Exception(msg) { }
        InvalidField(const std::string& msg) : herdstat::Exception(msg) { }
        virtual ~InvalidField() throw() { }
};

class BadOption : public herdstat::Exception
{
    public:
        BadOption() { }
        BadOption(const char *msg) : herdstat::Exception(msg) { }
        BadOption(const std::string &msg) : herdstat::Exception(msg) { }
        virtual ~BadOption() throw() { }
        virtual const char *what() const throw()
        {
            std::string s("Invalid option: ");
            s += this->message();
            return s.c_str();
        }
};

class FormatException : public herdstat::Exception
{
    public:
        FormatException() { }
        FormatException(const char *msg) : herdstat::Exception(msg) { }
        FormatException(const std::string &msg) : herdstat::Exception(msg) { }
        virtual ~FormatException() throw() { }
};

class CleanupException : public herdstat::Exception
{
    public:
        CleanupException(int code) : _code(code) { }
        virtual ~CleanupException() throw() { }
        int exit_code() const { return _code; }

    private:
        int _code;
};

class IOHandlerUnimplemented : public herdstat::Exception
{
    public:
        IOHandlerUnimplemented() { }
        IOHandlerUnimplemented(const char *msg) : herdstat::Exception(msg) { }
        IOHandlerUnimplemented(const std::string& msg) : herdstat::Exception(msg) { }
        virtual ~IOHandlerUnimplemented() throw() { }
        virtual const char *what() const throw()
        {
            if (this->message())
            {
                std::string s("Invalid I/O method: ");
                s+=this->message();
                return s.c_str();
            }
            return "";
        }
};

/* action handler exceptions */
class ActionException : public herdstat::BaseException { };
class HerdException   : public ActionException { };
class DevException    : public ActionException { };

#endif

/* vim: set tw=80 sw=4 et : */
