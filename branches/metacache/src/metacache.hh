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

struct metadata_T
{
    metadata_T() : is_category(false) { }
    metadata_T(const util::string &pa, const util::string &pk = "",
        bool c = false) : path(pa), pkg(pk), is_category(c) { }

    void dump(const std::ostream &);

    util::string path, pkg longdesc;
    std::vector<util::string> herds;
    herd_T devs;
    bool is_category;
};

/*
 * A cache of all metadata.xml's.
 */

class metacache_T : public util::cache_T<std::vector<metadata_T> >
{
    public:
        metacache_T();
        metacache_T(const util::string &portdir);

        virtual bool valid() const;
        virtual void fill();
        virtual void load();
        virtual void dump();

    private:
        util::string _portdir;
};

#endif

/* vim: set tw=80 sw=4 et : */
