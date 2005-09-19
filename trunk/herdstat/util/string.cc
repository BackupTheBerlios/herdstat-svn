/*
 * herdstat -- herdstat/util/string.cc
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
#include <map>
#include <cstdarg>
#include <cstring>
#include <cctype>
#include <locale>
#include <functional>

#include <herdstat/exceptions.hh>
#include <herdstat/util/string.hh>

/*****************************************************************************
 * Clean up the whitespace of the given string (collapse whitespace, remove  *
 * trailing/leading whitespace, and convert any \n's to spaces).             *
 *****************************************************************************/
struct BothSpaces
{
    bool operator()(char c1, char c2) const
    { return (std::isspace(c1) and std::isspace(c2)); }
};

std::string
util::tidy_whitespace(const std::string &s)
{
    if (s.empty())
	return s;

    std::string result;
    std::string::size_type pos;

    /* collapse whitespace */
    std::unique_copy(s.begin(), s.end(),
        std::back_inserter(result), BothSpaces());

    /* replace all newlines with a space */
    std::replace(result.begin(), result.end(), '\n', ' ');

    /* remove any leading whitespace */
    if ((pos = result.find_first_not_of(" \t")) != std::string::npos)
	result.erase(0, pos);

    /* remove any trailing whitespace */
    if ((pos = result.find_last_not_of(" \t")) != std::string::npos)
	result.erase(++pos);
	
    return result;
}
/*****************************************************************************/
std::string
util::sprintf(const char *fmt, ...)
{
    va_list v;
    va_start(v, fmt);
    std::string s(util::sprintf(fmt, v));
    va_end(v);
    return s;
}
/*****************************************************************************/
std::string
util::sprintf(const char *fmt, va_list v)
{
    char *buf;
    vasprintf(&buf, fmt, v);
    std::string s(buf);
    std::free(buf);
    return s;
}
/*****************************************************************************
 * HTMLify the given std::string (replace any occurrences of &,>,<)               *
 *****************************************************************************/
std::string
util::htmlify(const std::string &str)
{
    std::string result(str);
    std::map<std::string, std::string> sr;
    std::map<std::string, std::string>::iterator i, e;
    sr["&"] = "&amp;";
    sr[">"] = "&gt;";
    sr["<"] = "&lt;";

    i = sr.begin(), e = sr.end();
    for (; i != e ; ++i)
    {
	std::string::size_type pos, lpos = 0;
	while (true)
	{
	    pos = result.find(i->first, lpos);
	    if (pos == std::string::npos)
		break;

	    if (result.substr(pos, pos + i->second.length()) == i->second)
		result.replace(pos, i->first.length(), i->second, 0,
		    i->second.length());

	    lpos = ++pos;
	}
    }

    return result;
}
/*****************************************************************************
 * unHTMLify the given std::string (replace occurrences of &amp;,&gt;,&;lt;       *
 *****************************************************************************/
std::string
util::unhtmlify(const std::string &str)
{
    std::string result(str);
    std::map<std::string, std::string> sr;
    std::map<std::string, std::string>::iterator i, e;
    sr["&amp;"] = "&";
    sr["&gt;"] = ">";
    sr["&lt;"] = "<";

    i = sr.begin(), e = sr.end();
    for (; i != e ; ++i)
    {
	std::string::size_type pos, lpos = 0;
	while (true)
	{
	    pos = result.find(i->first, lpos);
	    if (pos == std::string::npos)
		break;

	    result.replace(pos, i->first.length(), i->second,
		0, i->second.length());

	    lpos = ++pos;
	}
    }

    return result;
}
/*****************************************************************************
 * Convert a vector of std::string to one std::string.                                 *
 *****************************************************************************/
std::string
util::join(const std::vector<std::string> &v,
           const std::string::value_type delim)
{
    std::string result;

    std::vector<std::string>::const_iterator i, end;
    for (i = v.begin(), end = v.end(); i != end ; ++i)
    {
        result += *i;
        if ((i+1) != end)
            result += delim;
    }

    return result;
}
/*****************************************************************************/
std::vector<std::string>
util::split(const std::string& str, const std::string::value_type delim,
            bool append_empty)
{
    std::vector<std::string> v;
    std::string::size_type pos, lpos = 0;
    
    while (true)
    {
	if ((pos = str.find(delim, lpos)) == std::string::npos)
	{
	    v.push_back(str.substr(lpos));
	    break;
	}

	/* don't append empty strings (two
	 * delimiters in a row were encountered) */
        const char * const sub(str.substr(lpos, pos - lpos).c_str());
	if (std::strlen(sub) > 0 or append_empty)
	    v.push_back(sub);

	lpos = ++pos;
    }
    return v;
}
/*****************************************************************************/
/* vim: set tw=80 sw=4 et : */
