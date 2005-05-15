/*
 * herdstat -- lib/string.cc
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

#include <algorithm>
#include <iterator>
#include <vector>
#include <cstdarg>
#include <cstring>

#ifdef UNICODE
# include <glib/gprintf.h>
# include <glibmm/unicode.h>
#else /* UNICODE */
# include <locale>
#endif /* UNICODE */

#include "string.hh"

/*****************************************************************************
 * Convert the given character to (lower|upper)case                          *
 *****************************************************************************/
#ifdef UNICODE
gunichar
util::tolower(const gunichar c)
{
    return Glib::Unicode::tolower(c);
}

gunichar
util::toupper(const gunichar c)
{
    return Glib::Unicode::toupper(c);
}
#else /* UNICODE */
char
util::tolower(const char c)
{
    return std::tolower(c, std::locale(""));
}

char
util::toupper(const char c)
{
    return std::toupper(c, std::locale(""));
}
#endif /* UNICODE */
/*****************************************************************************
 * Convert the given string to all lowercase.                                *
 *****************************************************************************/
util::string
util::lowercase(const util::string &s)
{
    if (s.empty())
	return "";

    util::string result;
    for (util::string::const_iterator i = s.begin() ; i != s.end() ; ++i)
        result.push_back(util::tolower(*i));
    
    return result;
}
/*****************************************************************************
 * Clean up the whitespace of the given string (collapse whitespace, remove  *
 * trailing/leading whitespace, and convert any \n's to spaces).             *
 *****************************************************************************/
bool
bothspaces(util::string::value_type c1,
           util::string::value_type c2)
{
#ifdef UNICODE
    return Glib::Unicode::isspace(c1) and
           Glib::Unicode::isspace(c2);
#else /* UNICODE */
    std::locale loc("");
    return std::isspace(c1, loc) and std::isspace(c2, loc);
#endif /* UNICODE */
}

util::string
util::tidy_whitespace(const util::string &s)
{
    if (s.empty())
	return "";

    util::string result;

    /* collapse whitespace */
    std::unique_copy(s.begin(), s.end(), std::back_inserter(result),
        bothspaces);

    /* remove any leading whitespace */
    util::string::size_type pos = result.find_first_not_of(" \t\n");
    if (pos != util::string::npos)
	result.erase(0, pos);

    /* convert any newlines in the middle to a space */
    util::string result2;
    util::string::iterator i;
    for (i = result.begin() ; i != result.end() ; ++i)
        result2.push_back(*i == '\n' ? ' ' : *i);

    /* remove any trailing whitespace */
    pos = result2.find_last_not_of(" \t\n");
    if (pos != util::string::npos)
	result2.erase(++pos);
	
    return result2;
}
/*****************************************************************************/
util::string
#ifdef UNICODE
util::sprintf(const gchar *fmt, ...)
#else /* UNICODE */
util::sprintf(const char *fmt, ...)
#endif /* UNICODE */
{
    va_list v;
    va_start(v, fmt);
    util::string s(util::sprintf(fmt, v));
    va_end(v);
    return s;
}
/*****************************************************************************/
util::string
#ifdef UNICODE
util::sprintf(const gchar *fmt, va_list v)
{
    gchar *buf;
    g_vasprintf(&buf, fmt, v);
#else /* UNICODE */
util::sprintf(const char *fmt, va_list v)
{
    char *buf;
    vasprintf(&buf, fmt, v);
#endif /* UNICODE */
    
    util::string s(buf);
    free(buf);
    return s;
}
/*****************************************************************************/
std::vector<util::string>
util::split(const util::string &str, const util::string::value_type delim)
{
    std::vector<util::string> vec;
    util::string::size_type pos, lpos = 0;
    
    while (true)
    {
	pos = str.find(delim, lpos);
	if (pos == util::string::npos)
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
/*****************************************************************************
 * Convert a vector of string to one string.                                 *
 *****************************************************************************/
util::string
util::stringify(const std::vector<util::string> &v,
                const util::string::value_type delim)
{
    util::string result;

    std::vector<util::string>::const_iterator i;
    for (i = v.begin() ; i != v.end() ; ++i)
        result += *i + delim;

    /* remove the extra delim */
    util::string::size_type pos = result.rfind(delim);
    if (pos != util::string::npos)
        result = result.substr(0, pos);
    
    return result;
}
/*****************************************************************************/
std::vector<util::string>
util::string::split(const util::string::value_type delim) const
{
    std::vector<util::string> vec;
    size_type pos, lpos = 0;
    
    while (true)
    {
	if ((pos = this->find(delim, lpos)) == this->npos)
	{
	    vec.push_back(this->substr(lpos));
	    break;
	}

	/* don't append empty strings (two
	 * delimiters in a row were encountered) */
	if (this->substr(lpos, pos - lpos).length() > 0)
	    vec.push_back(this->substr(lpos, pos - lpos));

	lpos = ++pos;
    }
    return vec;
}
/*****************************************************************************/

/* vim: set tw=80 sw=4 et : */
