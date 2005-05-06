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

#include "common.hh"

/* metadata cache location */
#define CACHE               LOCALSTATEDIR"/metadatas"
/* copy of PORTDIR/metadata/timestamp (if portdir was rsync'd) */
#define LASTSYNC            LOCALSTATEDIR"/lastsync"

/* rough number of metadata.xml's in the tree, updated from time to
 * time as it grows. Used for initial vector size so that a ton of
 * re-allocations can be prevented */
#define METADATA_RESERVE    8400
#define DEFAULT_EXPIRE      86400

/*
 * An instance of metadatas_T represents a list of every metadata.xml
 * in the portage tree that belongs to a package (so category metadata.xml's
 * are excluded).
 */

class metadatas_T : public util::cache_T
{
    private:
        util::string portdir;

    public:
        metadatas_T() : util::cache_T(CACHE, METADATA_RESERVE) { }
        metadatas_T(const util::string &p)
            : util::cache_T(CACHE, METADATA_RESERVE), portdir(p)
        { init(); }

        void set_portdir(const util::string &p) { portdir = p; init(); }

        virtual bool valid() const;
        virtual void fill();
};

#endif

/* vim: set tw=80 sw=4 et : */
