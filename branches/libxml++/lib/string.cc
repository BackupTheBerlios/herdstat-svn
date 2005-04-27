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

#include <locale>
#include <algorithm>
#include <iterator>
#include <vector>
#include <cstdarg>
#include <cstring>
#include <glibmm/unicode.h>

#include "string.hh"

/*
 * Given a string, convert all characters to lowercase
 */

util::string
util::lowercase(const util::string &s)
{
    if (s.empty())
	return "";

    util::string result;
    for (util::string::const_iterator i = s.begin() ; i != s.end() ; ++i)
    {
        result.push_back(Glib::Unicode::tolower(*i));
    }
    
    return result;
}

/*
 * Given a string, tidy whitespace.
 */

bool
bothspaces(gunichar c1, gunichar c2)
{
    return Glib::Unicode::isspace(c1) and
           Glib::Unicode::isspace(c2);
}

util::string
util::tidy_whitespace(const util::string &s)
{
    if (s.empty())
	return "";

    util::string result;

    /* collapse whitespace */
    std::unique_copy(s.begin(), s.end(), std::back_inserter(result), bothspaces);

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

/*
 * Return a string formatted with printf-like format specifier
 */
util::string
util::sprintf(const char *str, ...)
{
    va_list v;
    va_start(v, str);
    util::string s(util::sprintf(str, v));
    va_end(v);
    return s;
}

/*
 * Overloaded sprintf that takes a string and va_list
 */
util::string
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

    util::string s(buf);

#ifdef HAVE_VASPRINTF
    free(buf);
#endif

    return s;
}

/*
 * Given a string and a delimiter, split the string,
 * returning all the substrings in a vector
 */

std::vector<util::string>
util::split(const util::string &str, const char delim)
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

/*
 * Convert the given vector to one string
 * consisting of the elements delimited by
 * the specified delimiter.
 */

util::string
util::vec2str(const std::vector<util::string> &v, const char delim)
{
    util::string result;

    std::vector<util::string>::const_iterator i;
    for (i = v.begin() ; i != v.end() ; ++i)
        result += *i + delim;
    
    /* erase the extra delim */
    result.erase(result.length() - 1);

    return result;
}

std::vector<util::string>
util::string::split(const char delim) const
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

/* vim: set tw=80 sw=4 et : */
