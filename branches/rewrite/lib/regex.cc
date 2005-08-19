/*
 * herdstat -- lib/regex.cc
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

#include "regex.hh"
#include "util_exceptions.hh"

namespace util {
/*****************************************************************************/
bool
regex::operator== (const string_type &cmp) const
{
    assert(this->_compiled);

    int rv = regexec(&(this->_regex), cmp.c_str(), 0, NULL, this->_eflags);
    if (rv == REG_ESPACE)
        throw bad_regex_E(rv, &(this->_regex));

    return rv == 0;
}
/*****************************************************************************/
void
regex::assign(const string_type &regex, int cflags, int eflags)
{
    if (this->_compiled)
        this->cleanup();

    this->_str    = regex;
    this->_cflags = cflags;
    this->_eflags = eflags;

    /* compile regex */
    int rv = regcomp(&(this->_regex), regex.c_str(), this->_cflags);
    if (rv != 0)
        throw bad_regex_E(rv, &(this->_regex));

    this->_compiled = true;
}
/*****************************************************************************/
void
regex::cleanup()
{
    regfree(&(this->_regex));
    this->_compiled = false;
    this->_eflags = this->_cflags = 0;
    this->_str.clear();
}
/*****************************************************************************/
} // namespace util

/* vim: set tw=80 sw=4 et : */
