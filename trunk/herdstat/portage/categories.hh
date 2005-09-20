/*
 * herdstat -- herdstat/portage/categories.hh
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

#ifndef _HAVE_CATEGORIES_HH
#define _HAVE_CATEGORIES_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/**
 * @file categories.hh
 * @brief Defines the Categories class.
 */

#include <set>
#include <string>

namespace portage {

    class Categories
    {
        public:
            typedef std::set<std::string> container_type;
            typedef container_type::value_type value_type;
            typedef container_type::size_type size_type;
            typedef container_type::reference reference;
            typedef container_type::const_reference const_reference;
            typedef container_type::iterator iterator;
            typedef container_type::const_iterator const_iterator;

            Categories(bool validate = false);
            Categories(const std::string& portdir, bool validate = false);

            inline const_iterator begin() const;
            inline const_iterator end() const;
            inline const_iterator find(const value_type& v) const;
            inline size_type size() const;
            inline bool empty() const;

        private:
            void fill();

            const std::string& _portdir;
            const bool _validate;
            static bool _init;
            static container_type _s;
    };

    inline Categories::const_iterator Categories::begin() const
    { return _s.begin(); }
    inline Categories::const_iterator Categories::end() const
    { return _s.end(); }
    inline Categories::const_iterator Categories::find(const value_type& v) const
    { return _s.find(v); }
    inline Categories::size_type Categories::size() const { return _s.size(); }
    inline bool Categories::empty() const { return _s.empty(); }

} // namespace portage

#endif /* _HAVE_CATEGORIES_HH */

/* vim: set tw=80 sw=4 et : */
