/*
 * herdstat -- lib/util.cc
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
#include <algorithm>
#include <iterator>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <cerrno>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include "util.hh"

util::color_map_T::cmap util::color_map_T::cm;

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

	return (str1 == str2);
    }

    return false;
}

/*
 * getcwd() wrapper
 */

std::string
util::getcwd()
{
    char *pwd = ::getcwd(NULL, 0);
    if (not pwd)
	throw util::errno_E("getcwd");

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

	while (true)
	{
	    std::string::size_type begin = result.find("${", lpos);
	    if (begin == std::string::npos)
		break;

	    std::string::size_type end = result.find("}", begin);
	    if (end == std::string::npos)
		break;
	    
	    std::string s(result.substr(begin + 2, end - (begin + 2)));

	    vars.push_back(s);
	    lpos = ++end;
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
		result.replace(pos, s.length(), i->second, 0,
		    i->second.length());
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
		    result.replace(pos, s.length(), ebuild, 0, ebuild.length());

		/* ${PN} */
		else if (i->first == "PN")
		{
		    if ((p = ebuild.rfind('-')) != std::string::npos)
			ebuild = ebuild.substr(0, p);

		    result.replace(pos, s.length(), ebuild, 0, ebuild.length());
		}

		/* ${PV} */
		else if (i->first == "PV")
		{
		    if ((p = ebuild.rfind('-')) != std::string::npos)
			ebuild = ebuild.substr(p + 1);

		    result.replace(pos, s.length(), ebuild, 0, ebuild.length());
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

    std::auto_ptr<std::ifstream> f(new std::ifstream(path.c_str()));
    if (not (*f))
	throw util::bad_fileobject_E(path);

    std::string result;
    rcfile_T rc(*f);
    rcfile_T::rcfile_keys_T::iterator pos = rc.keys.find(var);
    if (pos != rc.keys.end())
	result = pos->second;

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

    std::auto_ptr<std::ifstream> f(new std::ifstream(path.c_str()));
    if (not (*f))
	throw util::bad_fileobject_E(path);

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
    if (util::is_file(to) and (unlink(to.c_str()) != 0))
	throw util::bad_fileobject_E(to);

    std::auto_ptr<std::ifstream> ffrom(new std::ifstream(from.c_str()));
    std::auto_ptr<std::ofstream> fto(new std::ofstream(to.c_str()));

    if (not (*ffrom))
	throw util::bad_fileobject_E(from);
    if (not (*fto))
	throw util::bad_fileobject_E(to);

    /* read from ffrom and write to fto */
    std::copy(std::istream_iterator<std::string>(*ffrom),
	std::istream_iterator<std::string>(),
	std::ostream_iterator<std::string>(*fto, "\n"));
}

/*
 * Copy then remove old
 */

void
util::move_file(const std::string &from, const std::string &to)
{
    util::copy_file(from, to);
    if (unlink(from.c_str()) != 0)
	throw util::bad_fileobject_E(from);
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
	throw util::bad_fileobject_E(dir);
    }

    std::string cmd;
    
    if (verbose)
	cmd = util::sprintf("%s -r -T5 -O %s '%s'", WGET, file, url);
    else
	cmd = util::sprintf("%s -rq -T5 -O %s '%s'", WGET, file, url);

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
    std::string result(path);
    std::string::size_type pos;

    /* chop all trailing /'s */
    while (result[result.length() - 1] == '/' and result.length() > 1)
	result.erase(result.length() - 1);

    if ((pos = result.rfind('/')) != std::string::npos)
	result = result.substr(pos + 1);

    return ( result.empty() ? "/" : result.c_str() );
}

const char *
util::basename(std::string const &path)
{
    return util::basename(path.c_str());
}

const char *
util::dirname(const char *path)
{
    std::string result(path);
    std::string::size_type pos;

    /* chop all trailing /'s */
    while (result[result.length() - 1] == '/' and result.length() > 1)
	result.erase(result.length() - 1);

    if ((pos = result.rfind('/')) != std::string::npos)
	result = result.substr(0, pos);

    return ( result.empty() ? "/" : result.c_str() );
}

const char *
util::dirname(std::string const &path)
{
    return util::dirname(path.c_str());
}
