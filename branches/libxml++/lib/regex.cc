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
# define _POSIX_C_SOURCE
#endif

#include <sys/types.h>
#include <regex.h>

#include "regex.hh"
#include "util_exceptions.hh"

/*
 * Compare the given string to our regular expression,
 * return true if it matches
 */

bool
util::regex_T::operator== (const std::string &cmp)
{
    assert(this->_compiled = true);

    int rv = regexec(&(this->_regex), cmp.c_str(), 0, NULL, this->_eflags);
    if (rv == REG_ESPACE)
        throw bad_regex_E(rv, &(this->_regex));

    return rv == 0;
}

/*
 * Assign a new regular expression, making sure to cleanup
 * any previous regex's
 */

void
util::regex_T::assign(const std::string &regex, int cflags, int eflags)
{
    if (this->_compiled)
        this->cleanup();

    this->_cflags = cflags;
    this->_eflags = eflags;

    /* compile regex */
    int rv = regcomp(&(this->_regex), regex.c_str(), this->_cflags);
    if (rv != 0)
        throw util::bad_regex_E(rv, &(this->_regex));

    this->_compiled = true;
}

/*
 * Cleanup
 */

void
util::regex_T::cleanup()
{
    regfree(&(this->_regex));
    this->_compiled = false;
    this->_eflags = 0;
    this->_cflags = 0;
}

/* vim: set tw=80 sw=4 et : */