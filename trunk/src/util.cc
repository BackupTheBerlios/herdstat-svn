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
#include <memory>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <cerrno>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "options.hh"
#include "exceptions.hh"
#include "util.hh"

std::map<color_name_T, std::string> util::color_map_T::cmap;

/*
 * Given an email address, return the username.
 */

std::string
util::get_user_from_email(const std::string &email)
{
    std::string::size_type pos = email.find('@');
    return (pos == std::string::npos ? email : email.substr(0, pos));
}

/*
 * Copy file from to copy to
 */

void
util::copy_file(const std::string &from, const std::string &to)
{
    /* remove to if it exists */
    if (util::is_file(to))
    {
	if (unlink(to.c_str()) != 0)
	    throw bad_fileobject_E("unlink: %s: %s", to.c_str(),
		strerror(errno));
    }

    std::auto_ptr<std::ifstream> ffrom(new std::ifstream(from.c_str()));
    std::auto_ptr<std::ostream> fto(new std::ofstream(to.c_str()));

    if (not (*ffrom))
	throw bad_fileobject_E("%s: %s", from.c_str(), strerror(errno));
    if (not (*fto))
	throw bad_fileobject_E("%s: %s", to.c_str(), strerror(errno));

    /* read from ffrom and write to fto */
    std::string s;
    while (std::getline(*ffrom, s))
	*fto << s << std::endl;
}

/*
 * Copy then remove old
 */

void
util::move_file(const std::string &from, const std::string &to)
{
    util::copy_file(from, to);
    if (unlink(from.c_str()) != 0)
	throw bad_fileobject_E("unlink: %s: %s", from.c_str(), strerror(errno));
}

/*
 * Try to determine user.  This is used for hilighting occurrences
 * of the user's username in ouput.  ECHANGELOG_USER is checked first
 * since a developer might use a different username than what his
 * developer username is.
 */
std::string
util::current_user()
{
    std::string user;
    std::string::size_type pos;

    char *result = getenv("ECHANGELOG_USER");
    if (result)
    {
	user = result;
	if ((pos = user.find('<')) != std::string::npos)
	{
	    user = user.substr(pos + 1);
	    if ((pos = user.find('>')) != std::string::npos)
		user = user.substr(0, pos);
	}
	else
	    user.clear();
    }
    else if ((result = getenv("USER")))
    {
	user = result;
	user += "@gentoo.org";
    }

    return (user.empty() ? "nobody@gentoo.org" : user);
}

/*
 * Try to determine the columns of the current terminal; use
 * a sensible default if we can't get it for some reason.
 */

std::string::size_type
util::getcols()
{
    std::string output;
    FILE *p = popen("stty size 2>/dev/null | cut -d' ' -f2", "r");
    if (p)
    {
	char line[5];
	if (std::fgets(line, sizeof(line) - 1, p) != NULL)
	    output = line;
	pclose(p);
    }

    return (output.empty() ? 78 : atoi(output.c_str()));
}

/*
 * Determine PORTDIR
 */

const char *
util::portdir()
{
    const char *make_conf = "/etc/make.conf";
    const char *make_glob = "/etc/make.globals";

    std::string portdir;

    if (util::is_file(make_conf))
    {
	std::auto_ptr<std::ifstream> f(new std::ifstream(make_conf));
	if (not (*f))
	    throw bad_fileobject_E("%s: %s", make_conf, strerror(errno));

	util::rcfile_T rc(*f);
	util::rcfile_keys_T::iterator pos = rc.keys.find("PORTDIR");
	if (pos != rc.keys.end())
	    portdir = pos->second;
    }

    if (portdir.empty() and util::is_file(make_glob))
    {
	std::auto_ptr<std::ifstream> f(new std::ifstream(make_glob));
	if (not (*f))
	    throw bad_fileobject_E("%s: %s", make_glob, strerror(errno));

	util::rcfile_T rc(*f);
	util::rcfile_keys_T::iterator pos = rc.keys.find("PORTDIR");
	if (pos != rc.keys.end())
	    portdir = pos->second;
    }

    return (portdir.empty() ? "/usr/portage" : portdir.c_str());
}

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
    {
	std::cerr << "You don't have proper permissions to write to "
	    << dir << "." << std::endl
	    << "Did you forget to add yourself to the portage group?"
	    << std::endl;
	throw bad_fileobject_E("%s: %s", dir, strerror(errno));
    }

    std::string cmd = util::sprintf("%s -rq -T5 -O %s '%s'", WGET, file, url);

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
    *(opts.outstream()) << util::sprintf(msg, v) << std::endl;
    va_end(v);
}

util::color_map_T::color_map_T()
{
    cmap[red]     = "\033[0;31m";
    cmap[green]   = "\033[0;32m";
    cmap[blue]    = "\033[1;34m";
    cmap[yellow]  = "\033[1;33m";
    cmap[orange]  = "\033[0;33m";
    cmap[magenta] = "\033[1;35m";
    cmap[cyan]    = "\033[1;36m";
    cmap[black]   = "\033[0;30m";
    cmap[white]   = "\033[0;1m";
    cmap[none]    = "\033[00m";
}

util::rcfile_T::rcfile_T(std::ifstream &stream)
{
    std::string line;
    std::string::size_type pos;

    while (std::getline(stream, line))
    {
        pos = line.find_first_not_of(" \t");
        if (pos != std::string::npos)
            line.erase(0, pos);

        if (line.length() < 1 or line[0] == '#')
            continue;

        pos = line.find('=');
        if (pos != std::string::npos)
        {
            std::string key = line.substr(0, pos);
            std::string val = line.substr(pos + 1);
            
            if (std::string::npos != (pos = key.find_first_not_of(" \t")))
                key.erase(0, pos);
            if (std::string::npos != (pos = val.find_first_not_of(" \t")))
                val.erase(0, pos);
            if (std::string::npos != (pos = key.find_last_not_of(" \t")))
                key.erase(++pos);
            if (std::string::npos != (pos = val.find_last_not_of(" \t")))
                val.erase(++pos);

	    if (std::string::npos != (pos = val.find_first_of("'\"")))
	    {
		val.erase(pos, pos + 1);
		if (std::string::npos != (pos = val.find_last_of("'\"")))
		    val.erase(pos, pos + 1);
	    }

            keys[key] = val;
        }
    }
}
