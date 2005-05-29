/*
 * herdstat -- src/pkgcache.hh
 * $Id: pkgcache.hh 360 2005-05-27 14:25:38Z ka0ttic $
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

#ifndef HAVE_PKGCACHE_HH
#define HAVE_PKGCACHE_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <map>
#include "common.hh"
#include "herds_xml.hh"

/*
 * Container for package query data.
 */
enum query_type { QUERYTYPE_DEV, QUERYTYPE_HERD };

class pkgQuery_T : public std::map<util::string, util::string>
{
    public:
        pkgQuery_T(const util::string &n, const util::string &w = "",
            bool dev = false) : info(n), query(n), with(w), id(0),
                                type(dev? QUERYTYPE_DEV : QUERYTYPE_HERD) { }

        void dump(std::ostream &) const;
        bool operator== (const pkgQuery_T &) const;
        const std::vector<util::string> make_list(const util::string &) const;

        herds_xml_T::devinfo_T info;    /* developer info (if type == dev) */
        util::string query;             /* query string */
        util::string with;              /* --with-{herd,maintainer} */
        int id;                         /* query id */
        std::time_t date;               /* query date */
        query_type type;                /* query type */
};

/*
 * Represents a cache of package query
 * results (produced by action_pkg_handler_T::search()).
 */

class pkgCache_T : public std::vector<pkgQuery_T * >
{
    public:
        ~pkgCache_T();

        void operator() (pkgQuery_T *);
        void load();
        void dump(std::ostream * = NULL);

        iterator find(const pkgQuery_T &);
        bool is_expired(pkgQuery_T *q) const { return this->is_expired(*q); }
        bool is_expired(const pkgQuery_T &) const;
        void sort_oldest_to_newest();

    protected:
        void compress();
        void decompress();
        void cleanse();
};

#endif

/* vim: set tw=80 sw=4 et : */
