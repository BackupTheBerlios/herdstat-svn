/*
 * herdstat -- src/metadatas.hh
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

#ifndef HAVE_METADATAS_HH
#define HAVE_METADATAS_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string>
#include <vector>

/* metadata cache location */
#define CACHE  LOCALSTATEDIR"/metadatas"
/* copy of PORTDIR/metadata/timestamp (if portdir was rsync'd) */
#define LASTSYNC LOCALSTATEDIR"/lastsync"

/*
 * rough number of metadata.xml's in the tree, updated from time to
 * time as it grows. Used for initial vector size so that a ton of
 * re-allocations can be prevented
 */
#define METADATA_RESERVE    8240

/*
 * An instance of metadatas_T represents a list of every metadata.xml
 * in the portage tree that belongs to a package (so category metadata.xml's
 * are excluded
 */

class metadatas_T
{
    private:
        bool cache_is_valid();
        void read_cache();
        void write_cache();
        void get();

        const std::string portdir;
        std::vector<std::string> _m;

    public:
        typedef std::vector<std::string>::iterator iterator;
        typedef std::vector<std::string>::size_type size_type;

        metadatas_T(const std::string &);

        iterator begin() { return _m.begin(); }
        iterator end() { return _m.end(); }
        size_type size() const { return _m.size(); }
};

#endif

/* vim: set tw=80 sw=4 et : */
