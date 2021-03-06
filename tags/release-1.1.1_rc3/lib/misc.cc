/*
 * herdstat -- lib/misc.cc
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

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <unistd.h>

#include "file.hh"
#include "string.hh"
#include "util_exceptions.hh"
#include "misc.hh"

/** static members **********************************************************/
util::color_map_T::cmap_T util::color_map_T::_cm;
/****************************************************************************/
void
#ifdef UNICODE
util::debug(const gchar *fmt, ...)
#else /* UNICODE */
util::debug(const char *fmt, ...)
#endif /* UNICODE */
{
#ifdef DEBUG
    va_list v;
    va_start(v, fmt);

    util::string s(util::sprintf(fmt, v));
    std::cerr << "!!! " << s << std::endl;
    
    va_end(v);
#endif /* DEBUG */
}
/****************************************************************************
 * Compare the md5sum of two files, returning true if they match.           *
 ****************************************************************************/
bool
util::md5check(const util::string &file1, const util::string &file2)
{
    util::string str1, str2;

    util::string cmd = "md5sum " + file1 + " " + file2;
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
	util::string::size_type pos;

	if ((pos = str1.find_first_of(" \t")) != util::string::npos)
	    str1 = str1.substr(0, pos);

	if ((pos = str2.find_first_of(" \t")) != util::string::npos)
	    str2 = str2.substr(0, pos);

	return (str1 == str2);
    }

    return false;
}
/****************************************************************************/
util::string
util::getcwd()
{
    char *pwd = ::getcwd(NULL, 0);
    if (not pwd)
	throw util::errno_E("getcwd");

    util::string s(pwd);
    std::free(pwd);
    return s;
}
/****************************************************************************
 * Given an email address, return the username.                             *
 ****************************************************************************/
util::string
util::get_user_from_email(const util::string &email)
{
    util::string::size_type pos = email.find('@');
    if (pos == util::string::npos)
        return email;

    return email.substr(0, pos);
}
/****************************************************************************
 * Try to determine user.  This is used for hilighting occurrences          *
 * of the user's username in ouput.  ECHANGELOG_USER is checked first       *
 * since a developer might use a different username than what his           *
 * developer username is.                                                   *
 ****************************************************************************/
util::string
util::current_user()
{
    util::string user;
    util::string::size_type pos;

    char *result = std::getenv("ECHANGELOG_USER");
    if (result)
    {
	user = result;
	if ((pos = user.find('<')) != util::string::npos)
	{
	    user = user.substr(pos + 1);
	    if ((pos = user.find('>')) != util::string::npos)
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
/****************************************************************************
 * Try to determine the columns of the current terminal; use                *
 * a sensible default if we can't get it for some reason.                   *
 ****************************************************************************/
util::string::size_type
util::getcols()
{
    util::string output;
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
	util::string::size_type pos;
	if ((pos = output.find(" ")) != util::string::npos)
	    return std::atoi(output.substr(pos).c_str());
    }

    return 78;
}
/****************************************************************************/
int
util::fetch(const util::string &url, const util::string &dir, bool verbose,
        bool timestamp)
{
    return util::fetch(url.c_str(), dir.c_str(), verbose, timestamp);
}
/****************************************************************************/
int
util::fetch(const char *url, const char *file, bool verbose, bool timestamp)
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

    util::string cmd;
    
    if (verbose and timestamp)
	cmd = util::sprintf("%s -r -N -t3 -T3 -O %s '%s'", WGET, file, url);
    else if (verbose)
        cmd = util::sprintf("%s -r -t3 -T3 -O %s '%s'", WGET, file, url);
    else if (timestamp)
        cmd = util::sprintf("%s -rq -N -t3 -T3 -O %s '%s'", WGET, file, url);
    else
	cmd = util::sprintf("%s -rq -t3 -T3 -O %s '%s'", WGET, file, url);

//    util::debug("executing '%s'", cmd.c_str());

    return std::system(cmd.c_str());
}
/****************************************************************************/
const util::string
util::format_date(const std::time_t &epoch, const char *fmt)
{
    util::string date;

    if (epoch != 0)
    {
        char buf[255] = { 0 };
        std::strftime(buf, sizeof(buf) - 1, fmt, std::localtime(&epoch));
        date.assign(buf);
    }

    return (date.empty() ? "(no date)" : date);
}

const util::string
util::format_date(const util::string &epoch, const char *fmt)
{
    return format_date(std::strtol(epoch.c_str(), NULL, 10), fmt);
}
/****************************************************************************/
template <typename T, typename U>
const std::vector<T>
util::map<T,U>::keys() const
{
    std::vector<T> v;
    typename map<T,U>::iterator i;
    for (i = this->begin() ; i != this->end() ; ++i)
        v.push_back(i->first);
    return v;
}
/****************************************************************************/
template <typename T, typename U>
const std::vector<U>
util::map<T,U>::values() const
{
    std::vector<U> v;
    typename map<T,U>::iterator i;
    for (i = this->begin() ; i != this->end() ; ++i)
        v.push_back(i->second);
    return v;
}
/****************************************************************************/

/* vim: set tw=80 sw=4 et : */
