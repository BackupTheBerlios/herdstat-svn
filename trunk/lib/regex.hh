/*
 * herdstat -- lib/regex.hh
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

#ifndef HAVE_REGEX_HH
#define HAVE_REGEX_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <cstddef>
#include <cassert>
#include <string>
#include <sys/types.h>
#include <regex.h>

/*
 * POSIX Regular Expressions
 * Initialize with regular expression
 * Use operator== to compare a string to the regular expression
 */

namespace util
{
    class regex_T
    {
        private:
            void cleanup();

            bool _compiled;
            int _cflags;
            int _eflags;
            regex_t _regex;

        public:
            regex_T() : _compiled(false), _cflags(0), _eflags(0) { }
            regex_T(const std::string &regex, int c = 0, int e = 0)
                : _compiled(false), _cflags(c), _eflags(e)
            { this->assign(regex, c, e); }
            ~regex_T() { if (this->_compiled) this->cleanup(); }

            void assign(const std::string &, int c = 0, int e = 0);
            bool operator== (const std::string &cmp);
    };
}

#endif

/* vim: set tw=80 sw=4 et : */
