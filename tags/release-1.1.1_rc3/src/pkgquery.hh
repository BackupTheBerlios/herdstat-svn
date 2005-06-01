/*
 * herdstat -- src/pkgquery.hh
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

#ifndef HAVE_PKGQUERY_HH
#define HAVE_PKGQUERY_HH 1

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
            bool dev = false) : info(n), query(n), with(w),
                                type(dev? QUERYTYPE_DEV : QUERYTYPE_HERD) { }

        void dump(std::ostream &) const;
        bool operator== (const pkgQuery_T &) const;
        std::vector<util::string> pkgs() const;

        herds_xml_T::devinfo_T info;
        util::string query, with;
        std::time_t date;
        query_type type;
};

#endif

/* vim: set tw=80 sw=4 et : */
