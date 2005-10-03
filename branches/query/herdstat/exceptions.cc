/*
 * herdstat -- herdstat/exceptions.cc
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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string>
#include <cerrno>
#include <cstdarg>
#include <herdstat/exceptions.hh>

Exception::Exception() : _buf(NULL)
{
}

Exception::Exception(const Exception& that)
    : _buf(NULL)
{
    *this = that;
}

Exception::Exception(const char *fmt, va_list v)
    : _buf(NULL)
{
    vasprintf(&_buf, fmt, v);
}

Exception::Exception(const char *fmt, ...)
    : _buf(NULL)
{
    va_start(_v, fmt);
    vasprintf(&_buf, fmt, _v);
    va_end(_v);
}

Exception::Exception(const std::string& fmt, ...)
    : _buf(NULL)
{
#ifdef HAVE_GCC4
    va_start(_v, fmt);
#else
    va_start(_v, fmt.c_str());
#endif
    vasprintf(&_buf, fmt.c_str(), _v);
    va_end(_v);
}

Exception::~Exception() throw()
{
    if (_buf)
        std::free(_buf);
}

Exception&
Exception::operator= (const Exception& that)
{
    if (_buf)
    {
        std::free(_buf);
        _buf = NULL;
    }

    if (that._buf)
        _buf = strdup(that._buf);

#if defined(va_copy)
    va_copy(_v, that._v);
#elif defined(__va_copy)
    __va_copy(_v, that._v);
#else
    std::memcpy(_v, that._v, sizeof(va_list));
#endif

    return *this;
}

const char *
Exception::what() const throw()
{
    return _buf;
}

ErrnoException::ErrnoException() : _code(errno)
{
}

ErrnoException::ErrnoException(const char *msg)
    : Exception(msg), _code(errno)
{
}

ErrnoException::ErrnoException(const std::string& msg)
    : Exception(msg), _code(errno)
{
}

const char *
ErrnoException::what() const throw()
{
    std::string s(this->message());
    std::string e(std::strerror(_code));
    if (s.empty())
        return e.c_str();
    if (e.empty())
        return s.c_str();
    return (s + ": " + e).c_str();
}

FileException::FileException()
{
}

FileException::FileException(const char *msg)
    : ErrnoException(msg)
{
}

FileException::FileException(const std::string& msg)
    : ErrnoException(msg)
{
}

BadCast::BadCast()
{
}

BadCast::BadCast(const char *msg)
    : Exception(msg)
{
}

BadCast::BadCast(const std::string& msg)
    : Exception(msg)
{
}

BadRegex::BadRegex()
    : _err(0), _re(NULL)
{
}

BadRegex::BadRegex(int e, const regex_t *re)
    : _err(e), _re(re)
{
}

BadRegex::BadRegex(const std::string& msg)
    : Exception(msg), _err(0), _re(NULL)
{
}

const char *
BadRegex::what() const throw()
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

BadDate::BadDate()
{
}

BadDate::BadDate(const char *msg)
    : Exception(msg)
{
}

BadDate::BadDate(const std::string& msg)
    : Exception(msg)
{
}

FetchException::FetchException()
{
}

FetchException::FetchException(const char *msg)
    : Exception(msg)
{
}

FetchException::FetchException(const std::string& msg)
    : Exception(msg)
{
}

MalformedEmail::MalformedEmail()
{
}

MalformedEmail::MalformedEmail(const char *msg)
    : Exception(msg)
{
}

MalformedEmail::MalformedEmail(const std::string& msg)
    : Exception(msg)
{
}

/* vim: set tw=80 sw=4 et : */
