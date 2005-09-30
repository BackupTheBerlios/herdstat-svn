/*
 * herdstat -- herdstat/debug.cc
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

#include <cstdarg>
#include <herdstat/exceptions.hh>
#include <herdstat/debug.hh>

std::string Debug::_path;
std::ofstream Debug::_stream;

Debug::Debug()
{
}

Debug::Debug(const std::string& path)
{
    _path.assign(path);

#ifdef DEBUG
    if (_stream.is_open())
        _stream.close();

    _stream.open(path.c_str());
    if (not _stream)
        throw FileException(path);
#endif
}

Debug::~Debug()
{
}

void
Debug::operator() (const char *fmt, ...)
{
#ifdef DEBUG
    char *buf;
    va_list v;
    va_start(v, fmt);
    vasprintf(&buf, fmt, v);
    va_end(v);
    _stream << "!!! " << buf << std::endl;
    std::free(buf);
#endif
}

void
Debug::operator() (const std::string& fmt, ...)
{
#ifdef DEBUG
    char *buf;
    va_list v;
#ifdef HAVE_GCC4
    va_start(v, fmt);
#else
    va_start(v, fmt.c_str());
#endif
    vasprintf(&buf, fmt.c_str(), v);
    va_end(v);
    _stream << buf << std::endl;
    std::free(buf);
#endif
}

/* vim: set tw=80 sw=4 et : */
