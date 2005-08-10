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
#include <map>
#include <cstdarg>
#include <cstring>

#ifdef UNICODE
# include <glib/gprintf.h>
# include <glibmm/unicode.h>
#else /* UNICODE */
# include <locale>
#endif /* UNICODE */

#include "string.hh"
#include "util_exceptions.hh"

inline std::vector<util::string>
util::string::split(const string::value_type delim, bool append_only) const
{
    return util::split(*this, delim, append_only);
}

/*****************************************************************************
 * Convert the given character to (lower|upper)case                          *
 *****************************************************************************/
util::string::value_type
util::tolower(const string::value_type c)
{
#ifdef UNICODE
    return Glib::Unicode::tolower(c);
#else
    return std::tolower(c, std::locale(""));
#endif
}
util::string::value_type
util::toupper(const string::value_type c)
{
#ifdef UNICODE
    return Glib::Unicode::toupper(c);
#else
    return std::toupper(c, std::locale(""));
#endif
}
bool
util::isdigit(const string::value_type c)
{
#ifdef UNICODE
    return Glib::Unicode::isdigit(c);
#else
    return std::isdigit(c, std::locale(""));
#endif
}
/*****************************************************************************
 * Convert the given string to all lowercase.                                *
 *****************************************************************************/
util::string
util::lowercase(const util::string &s)
{
    if (s.empty())
	return "";

    util::string result;
    util::string::const_iterator i = s.begin(), e = s.end();
    for (; i != e ; ++i)
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

/* NOTE: ideally we should use the below commented out code,
 * however, it is EXTREMELY FSCKING SLOW. */

//    std::locale loc("");
//    return std::isspace(c1, loc) and std::isspace(c2, loc);

    return std::isspace(c1) and std::isspace(c2);
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
    util::string::iterator i = result.begin(), e = result.end();
    for (; i != e ; ++i)
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
/*****************************************************************************
 * HTMLify the given string (replace any occurrences of &,>,<)               *
 *****************************************************************************/
util::string
util::htmlify(const util::string &str)
{
    util::string result(str);
    std::map<util::string, util::string> sr;
    std::map<util::string, util::string>::iterator i, e;
    sr["&"] = "&amp;";
    sr[">"] = "&gt;";
    sr["<"] = "&lt;";

    i = sr.begin(), e = sr.end();
    for (; i != e ; ++i)
    {
	util::string::size_type pos, lpos = 0;
	while (true)
	{
	    pos = result.find(i->first, lpos);
	    if (pos == util::string::npos)
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
 * unHTMLify the given string (replace occurrences of &amp;,&gt;,&;lt;       *
 *****************************************************************************/
util::string
util::unhtmlify(const util::string &str)
{
    util::string result(str);
    std::map<util::string, util::string> sr;
    std::map<util::string, util::string>::iterator i, e;
    sr["&amp;"] = "&";
    sr["&gt;"] = ">";
    sr["&lt;"] = "<";

    i = sr.begin(), e = sr.end();
    for (; i != e ; ++i)
    {
	util::string::size_type pos, lpos = 0;
	while (true)
	{
	    pos = result.find(i->first, lpos);
	    if (pos == util::string::npos)
		break;

	    result.replace(pos, i->first.length(), i->second,
		0, i->second.length());

	    lpos = ++pos;
	}
    }

    return result;
}
/*****************************************************************************
 * Convert a vector of string to one string.                                 *
 *****************************************************************************/
util::string
util::join(const std::vector<util::string> &v,
                const util::string::value_type delim)
{
    util::string result;

    std::vector<util::string>::const_iterator i = v.begin(), e = v.end();
    for (; i != e ; ++i)
    {
        result += *i;

        if ((i+1) != e)
            result += delim;
    }

    return result;
}
/*****************************************************************************/
std::vector<util::string>
util::split(const string &str, const string::value_type delim, bool append_empty)
{
    std::vector<util::string> vec;
    string::size_type pos, lpos = 0;
    
    while (true)
    {
	if ((pos = str.find(delim, lpos)) == string::npos)
	{
	    vec.push_back(str.substr(lpos));
	    break;
	}

	/* don't append empty strings (two
	 * delimiters in a row were encountered) */
	if (str.substr(lpos, pos - lpos).length() > 0)
	    vec.push_back(str.substr(lpos, pos - lpos));
        else if (append_empty)
            vec.push_back("");

	lpos = ++pos;
    }
    return vec;
}
/*****************************************************************************/
template <typename T>
T
util::destringify(const util::string &s)
{
    std::istringstream is(s.c_str());

    T v;
    is >> v;

    if (not is.eof())
        throw bad_cast_E("Failed to cast '"+s+"'.");

    return v;
}
/*****************************************************************************/
template <>
int
util::destringify<int>(const string &s)
{
    char *invalid;
    int result = std::strtol(s.c_str(), &invalid, 10);
    if (*invalid)
        throw bad_cast_E("Failed to cast '"+s+"' to int.");

    return result;
}
/*****************************************************************************/
template <>
long
util::destringify<long>(const string &s)
{
    char *invalid;
    long result = std::strtol(s.c_str(), &invalid, 10);
    if (*invalid)
        throw bad_cast_E("Failed to cast '"+s+"' to long.");

    return result;
}
/*****************************************************************************/
template <>
unsigned long
util::destringify<unsigned long>(const string &s)
{
    char *invalid;
    unsigned long result = std::strtoul(s.c_str(), &invalid, 10);
    if (*invalid or ((result == ULONG_MAX) and (errno == ERANGE)))
        throw bad_cast_E("Failed to cast '"+s+"' to unsigned long.");

    return result;
}
/*****************************************************************************/
template <>
double
util::destringify<double>(const string &s)
{
    char *invalid;
    double result = std::strtod(s.c_str(), &invalid);
    if (*invalid)
        throw bad_cast_E("Failed to cast '"+s+"' to double.");

    return result;
}
/*****************************************************************************/
template <>
float
util::destringify<float>(const string &s)
{
    char *invalid;
    float result = std::strtod(s.c_str(), &invalid);
    if (*invalid)
        throw bad_cast_E("Failed to cast '"+s+"' to float.");

    return result;
}
/*****************************************************************************/
template <>
bool
util::destringify<bool>(const string &s)
{
    if (s == "true" or s == "yes" or s == "on")
        return true;
    if (s == "false" or s == "no" or s == "off")
        return false;
    return destringify<int>(s);
}
/*****************************************************************************/
/* vim: set tw=80 sw=4 et : */
