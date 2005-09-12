/*
 * herdstat -- herdstat/exceptions.hh
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

#ifndef _HAVE_HERDSTAT_EXCEPTIONS_HH
#define _HAVE_HERDSTAT_EXCEPTIONS_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <exception>
#include <stdexcept>
#include <string>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <cerrno>
#include <sys/types.h>
#include <regex.h>

class BaseException : public std::exception { };

class Exception : public BaseException
{
    public:
        Exception() : _buf(NULL) { }

        Exception(const char *fmt, va_list v) : _buf(NULL)
        {
            vasprintf(&_buf, fmt, v);
        }
        
        Exception(const char *fmt, ...) : _buf(NULL)
        {
            va_start(_v, fmt);
            vasprintf(&_buf, fmt, _v);
            va_end(_v);
        }
        
        Exception(const std::string& fmt, ...) : _buf(NULL)
        {
#ifdef HAVE_GCC4
            va_start(_v, fmt);
#else
            va_start(_v, fmt.c_str());
#endif
            vasprintf(&_buf, fmt.c_str(), _v);
            va_end(_v);
        }

        virtual ~Exception() throw()
        {
            if (_buf)
                std::free(_buf);
        }

        virtual const char *what() const throw() { return _buf; }

    protected:
        const char *message() const { return _buf; }

    private:
        char *_buf;
        va_list _v;
};

class ErrnoException : public Exception
{
    public:
        ErrnoException() : _code(errno) { }
        ErrnoException(const char *msg) : Exception(msg), _code(errno) { }
        ErrnoException(const std::string& msg) : Exception(msg), _code(errno) { }

        virtual const char *what() const throw()
        {
            std::string s(this->message());
            std::string e(std::strerror(_code));
            if (s.empty())
                return e.c_str();
            if (e.empty())
                return s.c_str();
            return (s + ": " + e).c_str();
        }

        int code() const { return _code; }

    private:
        int _code;
};

class FileException : public ErrnoException
{
    public:
        FileException() { }
        FileException(const char *msg) : ErrnoException(msg) { }
        FileException(const std::string& msg) : ErrnoException(msg) { }
};

class BadCast : public Exception
{
    public:
        BadCast() { }
        BadCast(const char *msg) : Exception(msg) { }
        BadCast(const std::string& msg) : Exception(msg) { }
};

class BadRegex : public Exception
{
     public:
         BadRegex() : _err(0), _re(NULL) { }
         BadRegex(int e, const regex_t *re) : _err(e), _re(re) { }
         BadRegex(const std::string& s) : Exception(s), _err(0), _re(NULL) { }

         virtual const char *what() const throw()
         {
             if (not this->message() and (not _re or (_err == 0)))
                 return "";

             if (this->message())
             {
                 std::size_t len(regerror(_err, _re, NULL, 0));
                 char *buf = new char(len);
                 regerror(_err, _re, buf, len);
                 std::string s(buf);
                 delete buf;
                 return s.c_str();
             }
             
             return "";
         }
         
     private:
         int _err;
         const regex_t *_re;
};

class BadDate : public Exception
{
    public:
        BadDate() { }
        BadDate(const char *msg) : Exception(msg) { }
        BadDate(const std::string& msg) : Exception(msg) { }
};

class FetchException : public Exception
{
    public:
        FetchException() { }
        FetchException(const char *msg) : Exception(msg) { }
        FetchException(const std::string& msg) : Exception(msg) { }
};

class MalformedEmail : public Exception
{
    public:
        MalformedEmail() { }
        MalformedEmail(const char *msg) : Exception(msg) { }
        MalformedEmail(const std::string& msg) : Exception(msg) { }
};

#endif /* _HAVE_HERDSTAT_EXCEPTIONS_HH */

/* vim: set tw=80 sw=4 et : */
