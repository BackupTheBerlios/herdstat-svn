/*
 * herdstat -- src/util.hh
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
#ifndef HAVE_UTIL_HH
#define HAVE_UTIL_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string>
#include <map>
#include <vector>
#include <cstdarg>
#include <sys/time.h>

#ifndef PATH_MAX
# define PATH_MAX 4096
#endif

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
    int fetch(const std::string &, const std::string &);
    int fetch(const char *, const char *);
    bool is_dir(const char *);
    bool is_dir(const std::string &);
    bool is_file(const char *);
    bool is_file(const std::string &);
    const char *basename(const char *);
    const char *basename(std::string const &);
    const char *dirname(const char *);
    const char *dirname(std::string const &);
    std::string sprintf(const char *, ...);
    std::string sprintf(const char *, va_list);
    std::vector<std::string> splitstr(const std::string &, const char d = ' ');
    void debug_msg(const char *, ...);

    class timer_T
    {
	private:
	    struct timeval _begin, _end;
	    long ms;
	public:
	    void start() { gettimeofday(&_begin, NULL); }
	    void stop()
	    {
		gettimeofday(&_end, NULL);
		ms = _end.tv_sec - _begin.tv_sec;
		ms *= 1000;
		ms += (_end.tv_usec - _begin.tv_usec) / 1000;
	    }
	    long elapsed() const { return ms; }
    };

    class color_map_T
    {
	private:
	    static std::map<color_name_T, std::string> cmap;
	public:
	    color_map_T();
	    std::string &operator[](color_name_T c) { return cmap[c]; }
    };
}

#endif
