/*
 * herdstat -- herdstat/portage/package_list.hh
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

#ifndef _HAVE_PACKAGE_LIST_HH
#define _HAVE_PACKAGE_LIST_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/**
 * @file package_list.hh
 * @brief Defines the PackageList class.
 */

#include <string>
#include <vector>

#define PKGLIST_RESERVE            10130

namespace portage {

    class PackageList
    {
        public:
            typedef std::vector<std::string> container_type;
            typedef container_type::value_type value_type;
            typedef container_type::reference reference;
            typedef container_type::const_reference const_reference;
            typedef container_type::iterator iterator;
            typedef container_type::const_iterator const_iterator;
            typedef container_type::size_type size_type;

            PackageList();
            PackageList(const std::string& portdir, const std::vector<std::string>&
                overlays = std::vector<std::string>());

            /// Fill container.
            void fill();

            /// Implicit conversion to container_type&
            inline operator const container_type&() const;

            inline iterator begin();
            inline const_iterator begin() const;
            inline iterator end();
            inline const_iterator end() const;
            inline size_type size() const;
            inline bool empty() const;
            
            inline void reserve(size_type n);
            inline void push_back(const_reference v);
            inline iterator insert(iterator pos, const_reference v);
            template <class In>
            inline void insert(iterator pos, In begin, In end);

        private:
            const std::string& _portdir;
            const std::vector<std::string>& _overlays;

            static bool _init;
            static container_type _pkgs;
    };

    inline PackageList::operator const PackageList::container_type&() const
    { return _pkgs; }
    inline PackageList::iterator PackageList::begin()
    { return _pkgs.begin(); }
    inline PackageList::const_iterator PackageList::begin() const
    { return _pkgs.begin(); }
    inline PackageList::iterator PackageList::end()
    { return _pkgs.end(); }
    inline PackageList::const_iterator PackageList::end() const
    { return _pkgs.end(); }
    inline PackageList::size_type PackageList::size() const
    { return _pkgs.size(); }
    inline bool PackageList::empty() const { return _pkgs.empty(); }
    inline void PackageList::reserve(size_type n) { _pkgs.reserve(n); }
    inline void PackageList::push_back(const_reference v)
    { _pkgs.push_back(v); }
    inline PackageList::iterator
    PackageList::insert(iterator pos, const_reference v)
    { return _pkgs.insert(pos, v); }
    template <class In>
    inline void PackageList::insert(iterator pos, In begin, In end)
    { _pkgs.insert(pos, begin, end); }

} // namespace portage

#endif /* _HAVE_PACKAGE_LIST_HH */

/* vim: set tw=80 sw=4 et : */
