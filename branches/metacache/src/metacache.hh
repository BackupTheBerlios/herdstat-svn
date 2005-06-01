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
        typedef std::vector<string_type> herds_type;
        typedef std::vector<string_type> herd_type;
        typedef dev_attrs_T dev_type;

        metadata_T() : is_category(false) { }
        metadata_T(const string_type &pd, const string_type &pa,
            const string_type &pk = "", bool c = false)
            : path(pa), pkg(pk), is_category(c), _portdir(pd)
        { 
            if (this->pkg.empty())
                get_pkg_from_path();
        }

        bool dev_exists(const herd_type::value_type &) const;
        bool dev_exists(const util::regex_T &) const;
        bool herd_exists(const herds_type::value_type &) const;
        bool herd_exists(const util::regex_T &) const;

        void dump(const std::ostream &);

        string_type path, pkg, longdesc;
        herds_type herds;
        herd_type devs;
        bool is_category;

    private:
        void get_pkg_from_path();
        string_type _portdir;
};

/*
 * A cache of all metadata.xml's.
 */

class metacache_T : public util::cache_T<std::vector<metadata_T> >
{
    public:
        metacache_T(const util::string &portdir);

        virtual bool valid() const;
        virtual void fill();
        virtual void load();
        virtual void dump();
        void dump_xml();

        metadata_T parse_metadata(const util::path_T &);

    private:
        void push_back(const metadata_T &m) { this->_cache.push_back(m); }
        util::string _portdir;
};

#endif

/* vim: set tw=80 sw=4 et : */
