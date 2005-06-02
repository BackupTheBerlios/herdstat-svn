/*
 * herdstat -- src/metacache.hh
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

#ifndef HAVE_METACACHE_HH
#define HAVE_METACACHE_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "common.hh"
#include "herds.hh"

class metadata_T
{
    public:
        typedef util::string string_type;
        typedef std::vector<string_type> value_type;

        metadata_T() : is_category(false) { }
        metadata_T(const string_type &pd, const string_type &pa,
            const string_type &pk = "", bool c = false)
            : path(pa), pkg(pk), is_category(c), _portdir(pd)
        { 
            if (this->pkg.empty())
                get_pkg_from_path();
        }

        bool dev_exists(const string_type &) const;
        bool dev_exists(const util::regex_T &) const;
        bool herd_exists(const string_type &) const;
        bool herd_exists(const util::regex_T &) const;

        string_type path, pkg, longdesc;
        value_type herds, devs;
        bool is_category;

    private:
        void get_pkg_from_path();
        string_type _portdir;
};

/*
 * A cache of all metadata.xml's.
 */

class metacache_T : public std::vector<metadata_T>
{
    public:
        metacache_T(const util::string &portdir) : _portdir(portdir) { }

        bool valid() const;
        void fill();
        void load(std::vector<util::string> = std::vector<util::string>());
        void dump();

        metadata_T parse(const util::path_T &);

    private:
        util::string _portdir;
};

#endif

/* vim: set tw=80 sw=4 et : */
