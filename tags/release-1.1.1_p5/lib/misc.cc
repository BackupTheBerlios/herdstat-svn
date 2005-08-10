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

#ifdef USE_TERMCAP
# include <curses.h>
# include <term.h>
#endif /* USE_TERMCAP */

/** static members **********************************************************/
util::color_map_T::cmap_T util::color_map_T::_cm;

#ifdef USE_TERMCAP
static bool term_init = false;
static char term_info[2048];
#endif /* USE_TERMCAP */
/****************************************************************************/
util::color_map_T::string_type &
util::color_map_T::operator[] (const string_type &color)
{
    if (color == "red") return (*this)[red];
    else if (color == "green") return (*this)[green];
    else if (color == "blue") return (*this)[blue];
    else if (color == "yellow") return (*this)[yellow];
    else if (color == "orange") return (*this)[orange];
    else if (color == "magenta") return (*this)[magenta];
    else if (color == "cyan") return (*this)[cyan];
    else if (color == "black") return (*this)[black];
    else if (color == "white") return (*this)[white];
    else return (*this)[none];
}
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
#ifdef USE_TERMCAP

    if (not term_init)
    {
        const char *type = std::getenv("TERM");
        /* default to 'vt100' */
        if (not type)
        {
            if (setenv("TERM", "vt100", 0) != 0)
                throw util::va_msg_base_E("setenv(\"TERM\", \"vt100\") failed");
        }

        int result = tgetent(term_info, std::getenv("TERM"));
        if (result < 0)
            throw util::va_msg_base_E("Failed to access termcap database.");
        else if (result == 0)
            throw util::va_msg_base_E("Unknown terminal type '%s'", type);

        term_init = true;
    }

    int cols = tgetnum("co");
    if (cols > 0)
        return cols;

#else /* USE_TERMCAP */

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

#endif /* USE_TERMCAP */

    return 78;
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

/* vim: set tw=80 sw=4 et : */
