/*
 * herdstat -- herdstat/util/regex.cc
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

/* use POSIX regular expressions */
#ifndef _POSIX_C_SOURCE
# define _POSIX_C_SOURCE    200112
#endif

#include <sys/types.h>
#include <regex.h>

#include <herdstat/util/regex.hh>
#include <herdstat/exceptions.hh>

namespace util {
/*** static members **********************************************************/
const Regex::cflags Regex::extended = Regex::cflags(REG_EXTENDED);
const Regex::cflags Regex::icase = Regex::cflags(REG_ICASE);
const Regex::cflags Regex::nosub = Regex::cflags(REG_NOSUB);
const Regex::cflags Regex::newline = Regex::cflags(REG_NEWLINE);
const Regex::eflags Regex::notbol = Regex::eflags(REG_NOTBOL);
const Regex::eflags Regex::noteol = Regex::eflags(REG_NOTEOL);
/*****************************************************************************/
Regex::Regex()
    : _str(), _compiled(false), _cflags(0), _eflags(0), _regex()
{
}
/*****************************************************************************/
Regex::Regex(const Regex& that)
    : _str(), _compiled(false), _cflags(0), _eflags(0), _regex()
{
    *this = that;
}
/*****************************************************************************/
Regex::Regex(int c, int e)
    : _str(), _compiled(false), _cflags(c), _eflags(e), _regex()
{
}
/*****************************************************************************/
Regex::Regex(const std::string &regex, int c, int e)
    : _str(regex), _compiled(false), _cflags(c), _eflags(e), _regex()
{
    this->compile();
}
/*****************************************************************************/
Regex::~Regex()
{
    if (this->_compiled)
        this->cleanup();
}
/*****************************************************************************/
Regex&
Regex::operator= (const Regex& that)
{
    /* compiler-generated copy constructor/assignment operator won't do
     * since we need to free the old regex_t and compile the new one */

    this->assign(that._str, that._cflags, that._eflags);
    return *this;
}
/*****************************************************************************/
void
Regex::assign(const std::string& regex)
{
    if (this->_compiled)
        this->cleanup();

    this->_str.assign(regex);
    this->compile();
}
/*****************************************************************************/
void
Regex::assign(const std::string &regex, int c, int e)
{
    if (this->_compiled)
        this->cleanup();

    this->_str.assign(regex);
    this->_cflags = c;
    this->_eflags = e;

    this->compile();
}
/*****************************************************************************/
void
Regex::compile()
{
    if (this->_compiled)
        this->cleanup();

    int ret = regcomp(&(this->_regex), this->_str.c_str(), this->_cflags);
    if (ret != 0)
        throw BadRegex(ret, &(this->_regex));

    this->_compiled = true;
}
/*****************************************************************************/
void
Regex::cleanup()
{
    regfree(&(this->_regex));
    this->_compiled = false;
    this->_str.clear();
}
/*****************************************************************************/
} // namespace util

/* vim: set tw=80 sw=4 et : */
