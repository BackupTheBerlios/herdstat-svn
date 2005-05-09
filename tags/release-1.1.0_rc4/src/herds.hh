/*
 * herdstat -- src/herds.hh
 * $Id$
 * Copyright (c) 2005 Aaron Walker <ka0ttic at gentoo.org>
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

#ifndef HAVE_HERDS_HH
#define HAVE_HERDS_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "common.hh"
#include "devs.hh"

/*
 * Even though technically a herd is a group of packages, and not
 * developers, they are represented as groups of developers here,
 * with a couple attributes a herd would have (name, desc, etc).
 */

class herd_T  : public std::map<util::string, dev_attrs_T * >
{
    public:
        typedef util::string string_type;

        herd_T() { }
        herd_T(const string_type &n) : name(n) { }

        std::vector<key_type> keys();
        void display(std::ostream &);

        string_type name;
        string_type desc;
        string_type mail;
};

/*
 * A group of herds, mapped from herd name to a herd_T object
 */

class herds_T : public std::map<util::string, herd_T * >
{
    public:
        typedef util::string string_type;

        void display(std::ostream &);
        bool exists(const key_type &s)
        {
            return (this->find(s) != this->end());
        }
};

#endif

/* vim: set tw=80 sw=4 et : */
