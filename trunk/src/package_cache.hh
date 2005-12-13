/*
 * herdstat -- src/package_cache.hh
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

#ifndef _HAVE_SRC_PACKAGE_CACHE_HH
#define _HAVE_SRC_PACKAGE_CACHE_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <herdstat/portage/package_list.hh>

#include "options.hh"
#include "cache.hh"

class PackageCache : public Cache
{
    public:
        typedef herdstat::portage::PackageList container_type;
        typedef container_type::iterator iterator;
        typedef container_type::const_iterator const_iterator;
        typedef container_type::size_type size_type;
        typedef container_type::value_type value_type;

        virtual ~PackageCache() throw();

        inline operator const container_type&() const { return _pkgs; }

        inline const_iterator begin() const { return _pkgs.begin(); }
        inline const_iterator end() const { return _pkgs.end(); }
        inline size_type size() const { return _pkgs.size(); }
        inline bool empty() const { return _pkgs.empty(); }

    protected:
        virtual std::size_t cache_size() const;
        virtual const char * const name() const;
        virtual bool do_is_valid();
        virtual void do_load(herdstat::io::BinaryIStream& stream);
        virtual void do_dump(herdstat::io::BinaryOStream& stream);
        virtual void do_fill();

    private:
        friend const PackageCache& GlobalPkgCache(herdstat::util::ProgressMeter *);
        PackageCache(herdstat::util::ProgressMeter *progress);

        const std::string& _portdir;
        const std::vector<std::string>& _overlays;
        container_type _pkgs;
        herdstat::util::ProgressMeter *_spinner;
};

inline const PackageCache&
GlobalPkgCache(herdstat::util::ProgressMeter *spinner)
{
    static PackageCache p(spinner);
    return p;
}

#endif /* _HAVE_SRC_PACKAGE_CACHE_HH */

/* vim: set tw=80 sw=4 fdm=marker et : */
