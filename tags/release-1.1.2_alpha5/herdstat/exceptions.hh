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
#include <sys/types.h>
#include <regex.h>

class BaseException : public std::exception { };

class Exception : public BaseException
{
    public:
        Exception();
        Exception(const Exception& that);
        Exception(const char *fmt, va_list v);
        Exception(const char *fmt, ...);
        Exception(const std::string& fmt, ...);

        Exception& operator= (const Exception& that);

        virtual ~Exception() throw();
        virtual const char *what() const throw();

    protected:
        const char *message() const { return _buf; }

    private:
        char *_buf;
        va_list _v;
};

class ErrnoException : public Exception
{
    public:
        ErrnoException();
        ErrnoException(const char *msg);
        ErrnoException(const std::string& msg);
        virtual ~ErrnoException() throw() { }

        virtual const char *what() const throw();

        int code() const { return _code; }

    private:
        int _code;
};

class FileException : public ErrnoException
{
    public:
        FileException();
        FileException(const char *msg);
        FileException(const std::string& msg);
        virtual ~FileException() throw() { }
};

class BadCast : public Exception
{
    public:
        BadCast();
        BadCast(const char *msg);
        BadCast(const std::string& msg);
        virtual ~BadCast() throw() { }
};

class BadRegex : public Exception
{
     public:
         BadRegex();
         BadRegex(int e, const regex_t *re);
         BadRegex(const std::string& s);
         virtual ~BadRegex() throw() { }
         virtual const char *what() const throw();

     private:
         int _err;
         const regex_t *_re;
};

class BadDate : public Exception
{
    public:
        BadDate();
        BadDate(const char *msg);
        BadDate(const std::string& msg);
        virtual ~BadDate() throw() { }
};

class FetchException : public Exception
{
    public:
        FetchException();
        FetchException(const char *msg);
        FetchException(const std::string& msg);
        virtual ~FetchException() throw() { }
};

class MalformedEmail : public Exception
{
    public:
        MalformedEmail();
        MalformedEmail(const char *msg);
        MalformedEmail(const std::string& msg);
        virtual ~MalformedEmail() throw() { }
};

#endif /* _HAVE_HERDSTAT_EXCEPTIONS_HH */

/* vim: set tw=80 sw=4 et : */
