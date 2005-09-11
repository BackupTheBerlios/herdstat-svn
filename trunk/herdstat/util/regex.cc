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
/*****************************************************************************/
regex_T::regex_T()
    : _str(), _compiled(false), _cflags(0), _eflags(0), _regex()
{
}
/*****************************************************************************/
regex_T::regex_T(const std::string &regex, int cflags, int eflags)
    : _str(regex), _compiled(false), _cflags(cflags), _eflags(eflags), _regex()
{
    this->compile();
}
/*****************************************************************************/
regex_T::~regex_T()
{
    if (this->_compiled)
        this->cleanup();
}
/*****************************************************************************/
bool
regex_T::operator== (const std::string &cmp) const
{
    assert(this->_compiled);

    int ret = regexec(&(this->_regex), cmp.c_str(), 0, NULL, this->_eflags);
    if (ret == REG_ESPACE)
        throw BadRegex(ret, &(this->_regex));

    return (ret == 0);
}
/*****************************************************************************/
void
regex_T::assign(const std::string &regex, int cflags, int eflags)
{
    if (this->_compiled)
        this->cleanup();

    this->_str    = regex;
    this->_cflags = cflags;
    this->_eflags = eflags;

    this->compile();
}
/*****************************************************************************/
void
regex_T::compile()
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
regex_T::cleanup()
{
    regfree(&(this->_regex));
    this->_compiled = false;
    this->_eflags = this->_cflags = 0;
    this->_str.clear();
}
/*****************************************************************************/
} // namespace util

/* vim: set tw=80 sw=4 et : */
