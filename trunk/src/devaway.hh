/*
 * herdstat -- src/devaway.hh
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

#ifndef HAVE_DEVAWAY_HH
#define HAVE_DEVAWAY_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <map>
#include "common.hh"
#include "parsable.hh"

/*
 * Represents a list of developers who are away, and their
 * corresponding away message.
 */

class devaway_T : public parsable_T
{
    public:
        typedef std::map<util::string, util::string> value_type;
        typedef value_type::iterator iterator;
        typedef value_type::const_iterator const_iterator;
        typedef value_type::size_type size_type;

        devaway_T() { }

        const string_type &operator[] (const string_type &s)
        { return this->_away[s]; }

    protected:
        virtual void fetch();
        virtual void parse(const string_type &p);

        value_type _away;
};

#endif

/* vim: set tw=80 sw=4 et : */
