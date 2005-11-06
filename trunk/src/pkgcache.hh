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

#include "options.hh"
#include <herdstat/cachable.hh>
#include <herdstat/portage/package_list.hh>

class pkgcache : public herdstat::cachable
{
    public:
        typedef std::vector<std::string> container_type;
        typedef container_type::value_type value_type;
        typedef container_type::iterator iterator;
        typedef container_type::const_iterator const_iterator;
        typedef container_type::size_type size_type;

        virtual ~pkgcache();

        virtual bool valid() const;
        virtual void fill();
        virtual void load();
        virtual void dump();

        /// Implicit conversion to container_type
        operator const container_type&() const { return _pkgs; }

        iterator begin() { return _pkgs.begin(); }
        const_iterator begin() const { return _pkgs.begin(); }
        iterator end() { return _pkgs.end(); }
        const_iterator end() const { return _pkgs.end(); }
        size_type size() const { return _pkgs.size(); }
        bool empty() const { return _pkgs.empty(); }

    private:
        friend pkgcache& GlobalPkgCache();
        pkgcache();
        pkgcache(const std::string &portdir);
        pkgcache(const pkgcache&);
        pkgcache& operator= (const pkgcache&);

        Options& _options;
        int _reserve;
        const std::string& _portdir;
        const std::vector<std::string>& _overlays;
        herdstat::portage::PackageList _pkgs;
};

pkgcache& GlobalPkgCache();

#endif

/* vim: set tw=80 sw=4 fdm=marker et : */
