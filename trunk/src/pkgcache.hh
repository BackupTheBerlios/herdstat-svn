/*
 * herdstat -- src/pkgcache.hh
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

#ifndef HAVE_PKGCACHE_HH
#define HAVE_PKGCACHE_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "common.hh"

class pkgcache_T : public util::cache_T<std::vector<util::string> >
{
    public:
        pkgcache_T();
        pkgcache_T(const util::string &portdir);

        virtual bool valid() const;
        virtual void fill();
        virtual void load();
        virtual void dump();

        void set_portdir(const util::string &portdir)
        { this->_portdir = portdir; this->init(); }

    private:
        util::string _portdir;
};

#endif

/* vim: set tw=80 sw=4 et : */
