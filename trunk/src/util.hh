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

#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <cstdarg>
#include <cstdio>
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
    bool in_pkgdir();
    std::string getcwd();
    const char *get_ebuild_var(const std::string &, const std::string &,
	const std::string &);
    const char *ebuild_which(const std::string &, const std::string &);
    const char *get_var(const std::string &, const std::string &);
    std::map<std::string, std::string> get_vars(const std::string &,
	const std::vector<std::string> &);
    std::string lowercase(const std::string &);
    std::string tidy_whitespace(const std::string &);
    std::string get_user_from_email(const std::string &);
    void copy_file(const std::string &, const std::string &);
    void move_file(const std::string &, const std::string &);
    std::string current_user();
    std::string::size_type getcols();
    const char *portdir();
    int fetch(const std::string &, const std::string &, bool);
    int fetch(const char *, const char *, bool);
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
	    void start()
	    {
		ms = 0;
		gettimeofday(&_begin, NULL);
	    }

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

    class rcfile_T
    {
	public:
	    typedef std::map<std::string, std::string> rcfile_keys_T;
	    rcfile_T(std::ifstream &);
	    void dump(std::ostream &);
	    rcfile_keys_T keys;
    };

    class progress_T
    {
	private:
	    float cur, step;

	public:
	    progress_T() : cur(0) { }
	    void start(unsigned m)
	    {
		step = 100.0 / m;
		std::printf("  0%%");
	    }

	    void operator++ ()
	    {
		int inc = static_cast<int>(cur += step);
		if (inc < 10)
		    std::printf("\b\b%.1d%%", inc);
		else if (inc < 100)
		    std::printf("\b\b\b%.2d%%", inc);
		else
		    std::printf("\b\b\b\b%.3d%%", inc);
		std::fflush(stdout);
	    }
    };
}

#endif
