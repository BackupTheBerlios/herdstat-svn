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

#include <fstream>
#include <string>
#include <map>
#include <memory>
#include <iterator>
#include <algorithm>
#include <locale>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <cerrno>
#include <cctype>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include "options.hh"
#include "exceptions.hh"
#include "util.hh"

std::map<color_name_T, std::string> util::color_map_T::cmap;

/*
 * Compare the md5sum of two files returning true if they match.
 */

bool
util::md5check(const std::string &file1, const std::string &file2)
{
    std::string str1, str2;

    std::string cmd = "md5sum " + file1 + " " + file2;
    FILE *p = popen(cmd.c_str(), "r");
    if (p)
    {
	char line[PATH_MAX + 40];
	if (std::fgets(line, sizeof(line) - 1, p) != NULL)
	    str1 = line;
	if (std::fgets(line, sizeof(line) - 1, p) != NULL)
	    str2 = line;
	pclose(p);
    }

    if (not str1.empty() and not str2.empty())
    {
	std::string::size_type pos;

	if ((pos = str1.find_first_of(" \t")) != std::string::npos)
	    str1 = str1.substr(0, pos);

	if ((pos = str2.find_first_of(" \t")) != std::string::npos)
	    str2 = str2.substr(0, pos);

	util::debug_msg("md5sum (%s): %s", file1.c_str(), str1.c_str());
	util::debug_msg("md5sum (%s): %s", file2.c_str(), str2.c_str());

	return str1 == str2;
    }

    return false;
}

/*
 * Try to determine if the current directory is
 * a valid package directory.
 */

bool
util::in_pkgdir()
{
    const char *pwd = util::getcwd().c_str();
    DIR *dir = NULL;
    struct dirent *d = NULL;
    bool ebuild = false, filesdir = false;

    if (not (dir = opendir(pwd)))
	throw bad_fileobject_E(pwd);

    while ((d = readdir(dir)))
    {
	char *s = NULL;
	if ((s = std::strrchr(d->d_name, '.')))
	{
	    if (std::strcmp(++s, "ebuild") == 0)
		ebuild = true;
	}   
	else if (std::strcmp(d->d_name, "files") == 0)
	    filesdir = true;
    }

    closedir(dir);
    return ebuild and filesdir;
}

std::string
util::getcwd()
{
    char *pwd = ::getcwd(NULL, 0);
    if (not pwd)
	throw errno_error_E("getcwd");

    std::string s(pwd);
    std::free(pwd);
    return s;
}

/* 
 * Given the portdir, a package, and a variable name,
 * try to retrieve the value of the specified variable
 * from the ebuild.
 */

const char *
util::get_ebuild_var(const std::string &portdir,
		     const std::string &pkg,
		     const std::string &var)
{
    std::string ebuild = ebuild_which(portdir, pkg);
    std::string result = get_var(ebuild, var);

    /* Handle HOMEPAGE specially, doing our best to try
     * simple variable substituion; definitely does not 
     * work for all cases (in which case it's left alone) */
    if (var == "HOMEPAGE" and (result.find('$') != std::string::npos))
    {
	std::vector<std::string> vars;
	std::string::size_type lpos = 0;

	util::debug_msg("Parsing HOMEPAGE (%s)", result.c_str());

	while (true)
	{
	    std::string::size_type begin = result.find("${", lpos);
	    if (begin == std::string::npos)
		break;

	    std::string::size_type end = result.find("}", begin);
	    if (end == std::string::npos)
		break;
	    
	    std::string s(result.substr(begin + 2, end - (begin + 2)));

	    util::debug_msg("Found var '%s'", s.c_str());
	    vars.push_back(s);
	    lpos = ++begin;
	}

	std::map<std::string, std::string> varmap = get_vars(ebuild, vars);
	std::map<std::string, std::string>::iterator i;
	for (i = varmap.begin() ; i != varmap.end() ; ++i)
	{
	    std::string s("${" + i->first + "}");
	    std::string::size_type pos = result.find(s);
	    if (pos == std::string::npos)
		continue;

	    if (not i->second.empty())
	    {
		util::debug_msg("Replacing '%s' with '%s'", s.c_str(),
		    i->second.c_str());
		result.replace(pos, s.length(), i->second, 0,
		    i->second.length());
	    }
	    else
	    {
		/* chop path */
		std::string::size_type p = ebuild.rfind('/');
		if (p != std::string::npos)
		    ebuild = ebuild.substr(p + 1);

		/* chop .ebuild */
		if ((p = ebuild.rfind(".ebuild")) != std::string::npos)
		    ebuild = ebuild.substr(0, p);

		/* chop revision */
		if ((p = ebuild.rfind("-r")) != std::string::npos)
		    ebuild = ebuild.substr(0, p);

		/* ${P} */
		if (i->first == "P")
		{
		    result.replace(pos, s.length(), ebuild, 0, ebuild.length());
		    util::debug_msg("Replacing '${P}' with '%s'", ebuild.c_str());
		}

		/* ${PN} */
		else if (i->first == "PN")
		{
		    if ((p = ebuild.rfind('-')) != std::string::npos)
			ebuild = ebuild.substr(0, p);

		    result.replace(pos, s.length(), ebuild, 0, ebuild.length());
		    util::debug_msg("Replacing '${PN}' with '%s'", ebuild.c_str());
		}

		/* ${PV} */
		else if (i->first == "PV")
		{
		    if ((p = ebuild.rfind('-')) != std::string::npos)
			ebuild = ebuild.substr(p + 1);

		    result.replace(pos, s.length(), ebuild, 0, ebuild.length());
		    util::debug_msg("Replacing '${PV}' with '%s'", ebuild.c_str());
		}
	    }
	}
    }

    return result.c_str();
}

/*
 * Given a file, return the value of the specified variable
 * (if it exists). Variable will only be found if it's in
 * the form of VAR=value or VAR="value".
 */

const char *
util::get_var(const std::string &path, const std::string &var)
{
    if (path.empty() or var.empty())
	return "";

    util::debug_msg("get_var: opening '%s'", path.c_str());

    std::auto_ptr<std::ifstream> f(new std::ifstream(path.c_str()));
    if (not (*f))
	throw bad_fileobject_E(path);

    std::string result;
    rcfile_T rc(*f);
    rcfile_T::rcfile_keys_T::iterator pos = rc.keys.find(var);
    if (pos != rc.keys.end())
	result = pos->second;

    util::debug_msg("Retrieved %s from %s: '%s'", var.c_str(),
	path.c_str(), result.c_str());

    return result.c_str();
}

/*
 * Plural version of get_var()
 */

std::map<std::string, std::string>
util::get_vars(const std::string &path, const std::vector<std::string> &vars)
{
    std::map<std::string, std::string> varmap;

    if (path.empty() or vars.empty())
	return varmap;

    util::debug_msg("get_vars: opening '%s'", path.c_str());

    std::auto_ptr<std::ifstream> f(new std::ifstream(path.c_str()));
    if (not (*f))
	throw bad_fileobject_E(path);

    rcfile_T rc(*f);

    std::vector<std::string>::const_iterator i;
    for (i = vars.begin() ; i != vars.end() ; ++i)
    {
	rcfile_T::rcfile_keys_T::iterator pos = rc.keys.find(*i);
	if (pos != rc.keys.end())
	    varmap[*i] = pos->second;
	else
	    varmap[*i] = "";
    }

    return varmap;
}

/*
 * Do our best to guess the latest ebuild of the specified
 * package. TODO: write an actual version parsing class since
 * this often produces incorrect results.
 */

const char *
util::ebuild_which(const std::string &portdir, const std::string &pkg)
{
    DIR *dir = NULL;
    struct dirent *d = NULL;
    const std::string path = portdir + "/" + pkg;
    std::vector<std::string> ebuilds;
    std::vector<std::string>::iterator e;

    /* open package directory */
    if (not (dir = opendir(path.c_str())))
    {
	std::cerr << "failed to open dir '" << path << "'." << std::endl;
	return "";
    }

    /* read package directory looking for ebuilds */
    while ((d = readdir(dir)))
    {
	char *s = NULL;
	if ((s = std::strrchr(d->d_name, '.')))
	    if (std::strcmp(++s, "ebuild") == 0)
		ebuilds.push_back(path + "/" + d->d_name);
    }

    closedir(dir);

    if (ebuilds.empty())
	return "";

    std::sort(ebuilds.begin(), ebuilds.end());
    util::debug_msg("ebuild_which(%s) == '%s'", pkg.c_str(),
	ebuilds.back().c_str());
    return ebuilds.back().c_str();
}

/*
 * Given a string, convert all characters to lowercase
 */

std::string
util::lowercase(const std::string &s)
{
    if (s.empty())
	return "";

    std::string result(s);
    for (std::string::iterator i = result.begin() ; i != result.end() ; ++i)
	*i = std::tolower(*i, std::locale(optget("locale", std::string).c_str()));
    
    return result;
}

/*
 * Given a string, tidy whitespace.
 */

bool
bothspaces(char c1, char c2)
{
    std::locale loc = std::locale(optget("locale", std::string).c_str());
    return std::isspace(c1, loc) and std::isspace(c2, loc);
}

std::string
util::tidy_whitespace(const std::string &s)
{
    if (s.empty())
	return "";

    std::string result;

    /* collapse whitespace */
    std::unique_copy(s.begin(), s.end(), std::back_inserter(result), bothspaces);

    /* remove any leading whitespace */
    std::string::size_type pos = result.find_first_not_of(" \t\n");
    if (pos != std::string::npos)
	result.erase(0, pos);

    /* convert any newlines in the middle to a space */
    std::replace(result.begin(), result.end(), '\n', ' ');

    /* remove any trailing whitespace */
    pos = result.find_last_not_of(" \t\n");
    if (pos != std::string::npos)
	result.erase(++pos);
	
    return result;
}

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
	if (unlink(to.c_str()) != 0)
	    throw bad_fileobject_E(to);

    std::auto_ptr<std::ifstream> ffrom(new std::ifstream(from.c_str()));
    std::auto_ptr<std::ostream> fto(new std::ofstream(to.c_str()));

    if (not (*ffrom))
	throw bad_fileobject_E(from);
    if (not (*fto))
	throw bad_fileobject_E(to);

    util::debug_msg("copying file '%s' to '%s'", from.c_str(), to.c_str());

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
	throw bad_fileobject_E(from);
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
    FILE *p = popen("stty size 2>/dev/null", "r");
    if (p)
    {
	char line[10];
	if (std::fgets(line, sizeof(line) - 1, p) != NULL)
	    output = line;
	pclose(p);
    }

    if (not output.empty())
    {
	std::string::size_type pos;
	if ((pos = output.find(" ")) != std::string::npos)
	    return std::atoi(output.substr(pos).c_str());
    }

    return 78;
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
	portdir = get_var(make_conf, "PORTDIR");

    if (portdir.empty() and util::is_file(make_glob))
	portdir = get_var(make_glob, "PORTDIR");

    /* environment overrides all */
    char *result = getenv("PORTDIR");
    if (result)
	portdir = result;

    return (portdir.empty() ? "/usr/portage" : portdir.c_str());
}

/*
 * Download the specified file to the specified directory
 */

int
util::fetch(const std::string &url, const std::string &dir, bool verbose)
{
    return util::fetch(url.c_str(), dir.c_str(), verbose);
}

int
util::fetch(const char *url, const char *file, bool verbose)
{
    const char *dir = util::dirname(file);

    /* we have permission to write? */
    if (access(dir, W_OK) != 0)
    {
	std::cerr << "You don't have proper permissions to write to "
	    << dir << "." << std::endl
	    << "Did you forget to add yourself to the portage group?"
	    << std::endl;
	throw bad_fileobject_E(dir);
    }

    std::string cmd;
    
    if (verbose)
	cmd = util::sprintf("%s -r -T5 -O %s '%s'", WGET, file, url);
    else
	cmd = util::sprintf("%s -rq -T5 -O %s '%s'", WGET, file, url);

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
    std::size_t path_len = std::strlen(path);
    std::size_t base_len = std::strlen(util::basename(path));
    std::size_t len = (path[path_len - 1] == '/' ? 2 : 1);
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
    if (not optget("debug", bool))
	return;
    
    va_list v;
    va_start(v, msg);
    
    std::string s = util::sprintf(msg, v);

    /* make ASCII colors visible - TODO: anyway to escape them?
     * simply inserting a '\' before it doesnt work... */
    std::string::size_type pos = s.find("\033");
    if (pos != std::string::npos)
	s.erase(pos, 1);

    *(optget("outstream", std::ostream *)) << "!!! " << s << std::endl;
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

//    if (optget("debug", bool))
//        dump(std::cout);
}

void
util::rcfile_T::dump(std::ostream &stream)
{
    rcfile_keys_T::iterator k;
    for (k = keys.begin() ; k != keys.end() ; ++k)
	stream << "Key: '" << k->first << "', Value: '" << k->second << "'."
	    << std::endl;
}
