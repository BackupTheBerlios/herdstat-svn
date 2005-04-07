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

    char *result = std::getenv("ECHANGELOG_USER");
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
    else if ((result = std::getenv("USER")))
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
