/*
 * herdstat -- src/util.cc
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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <iostream>
#include <string>
#include <map>
#include <utility>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <cerrno>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "util.hh"
#include "options.hh"
#include "exceptions.hh"

std::map<color_name_T, std::string> util::color_map_T::cmap;

/*
 * Download the specified file to the specified directory
 */

int
util::fetch(const std::string &url, const std::string &dir)
{
    return util::fetch(url.c_str(), dir.c_str());
}

int
util::fetch(const char *url, const char *file)
{
    const char *dir = util::dirname(file);

    /* we have permission to write */
    if (access(dir, W_OK) != 0)
	throw bad_fileobject_E("%s: %s", dir, strerror(errno));

    std::string cmd = util::sprintf("%s -rq -t3 -T5 -O %s '%s'", WGET, file, url);

    util::debug_msg("Executing '%s'", cmd.c_str());

    return std::system(cmd.c_str());
}


/*
 * Is the specified path a directory?
 */
bool
util::is_dir(const char *path)
{
    struct stat s;
    if (stat(path, &s) != 0)
	return false;
    return S_ISDIR(s.st_mode);
}

/*
 * Overloaded is_dir that takes a std::string
 */

bool
util::is_dir(const std::string &path)
{
    return is_dir(path.c_str());
}

/*
 * Is the specified path a regular file?
 */

bool
util::is_file(const char *path)
{
    struct stat s;
    if (stat(path, &s) != 0)
	return false;
    return S_ISREG(s.st_mode);
}

/*
 * Overloaded is_file that takes a std::string
 */

bool
util::is_file(const std::string &path)
{
    return is_file(path.c_str());
}

const char *
util::basename(const char *path)
{
    std::string s = path;

    /* chop any trailing /'s */
    while (s[s.length() - 1] == '/' and s.length() > 1)
	s.erase(s.length() - 1);

    char *p = std::strrchr(s.c_str(), '/');
    return (p ? (*(p + 1) != '\0' ? p + 1 : p) : path);
}

const char *
util::basename(std::string const &path)
{
    return util::basename(path.c_str());
}

const char *
util::dirname(const char *path)
{
    std::string s = path;
    size_t path_len = strlen(path);
    size_t base_len = strlen(util::basename(path));
    size_t len = (path[path_len - 1] == '/' ? 2 : 1);
    len = path_len - base_len - (path_len - base_len - len == 0 ? len - 1 : len);

    return s.substr(0, len).c_str();
}

const char *
util::dirname(std::string const &path)
{
    return util::dirname(path.c_str());
}

/*
 * Return a string formatted with printf-like format specifier
 */
std::string
util::sprintf(const char *str, ...)
{
    va_list v;
    va_start(v, str);
    std::string s(util::sprintf(str, v));
    va_end(v);
    return s;
}

/*
 * Overloaded sprintf that takes a string and va_list
 */
std::string
util::sprintf(const char *str, va_list v)
{
#ifdef HAVE_VASPRINTF
    char *buf;
    vasprintf(&buf, str, v);
#else
    char buf[4096] = { 0 };
# ifdef HAVE_VSNPRINTF
    vsnprintf(buf, sizeof(buf), str, v);
# else
    vsprintf(buf, str, v);
# endif
#endif

    std::string s(buf);

#ifdef HAVE_VASPRINTF
    free(buf);
#endif

    return s;
}

/*
 * Given a string and a delimiter, split the string,
 * returning all the substrings in a vector
 */
std::vector<std::string>
util::splitstr(const std::string &str, const char delim)
{
    std::vector<std::string> vec;
    std::string::size_type pos, lpos = 0;
    
    while (true)
    {
	pos = str.find(delim, lpos);
	if (pos == std::string::npos)
	{
	    vec.push_back(str.substr(lpos));
	    break;
	}

	/* don't append empty strings (two
	 * delimiters in a row were encountered) */
	if (str.substr(lpos, pos - lpos).length() > 0)
	    vec.push_back(str.substr(lpos, pos - lpos));

	lpos = ++pos;
    }
    return vec;
}

/*
 * I hate to bring in options_T just for this, but it's either here
 * or make the caller declare an options_T instance and pass a bool value
 */
void
util::debug_msg(const char *msg, ...)
{
    options_T opts;
    if (not opts.debug())
	return;
    
    va_list v;
    va_start(v, msg);
    std::cout << util::sprintf(msg, v) << std::endl;
    va_end(v);
}

util::color_map_T::color_map_T()
{
    cmap.insert(std::make_pair(red,    "\033[0;31m"));
    cmap.insert(std::make_pair(green,  "\033[0;32m"));
    cmap.insert(std::make_pair(blue,   "\033[1;34m"));
    cmap.insert(std::make_pair(yellow, "\033[1;33m"));
    cmap.insert(std::make_pair(orange, "\033[0;33m"));
    cmap.insert(std::make_pair(magenta,"\033[1;35m"));
    cmap.insert(std::make_pair(cyan,   "\033[1;36m"));
    cmap.insert(std::make_pair(black,  "\033[0;30m"));
    cmap.insert(std::make_pair(white,  "\033[0;1m"));
    cmap.insert(std::make_pair(none,   "\033[00m"));
}
