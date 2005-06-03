/*
 * herdstat -- src/metadata.hh
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

#ifndef HAVE_METADATA_HH
#define HAVE_METADATA_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "common.hh"
#include "herds.hh"

class metadata_T
{
    public:
        typedef util::string string_type;
        typedef std::vector<string_type> herds_type;
        typedef herd_T herd_type;

        metadata_T() : is_category(false) { }
        metadata_T(const string_type &pd, const string_type &pa,
            const string_type &pk = "", bool c = false)
            : is_category(c), path(pa), pkg(pk), portdir(pd)
        {
            if (this->pkg.empty())
                this->get_pkg_from_path();
        }

        void get_pkg_from_path();
        void display() const;
        bool dev_exists(const string_type &) const;
        bool dev_exists(const util::regex_T &) const;
        bool herd_exists(const string_type &) const;
        bool herd_exists(const util::regex_T &) const;

        bool is_category;
        string_type path, pkg, longdesc, portdir;
        herds_type herds;
        herd_type  devs;
};

#endif

/* vim: set tw=80 sw=4 et : */
