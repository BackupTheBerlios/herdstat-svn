/*
 * herdstat -- herdstat/portage/portage_misc.hh
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

#ifndef HAVE_PORTAGE_MISC_HH
#define HAVE_PORTAGE_MISC_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <set>
#include <herdstat/exceptions.hh>
#include <herdstat/util/file.hh>
#include <herdstat/util/misc.hh>
#include <herdstat/portage/config.hh>

#define CATEGORIES "/profiles/categories"
#define CATEGORIES_USER "/etc/portage/categories"

namespace portage
{
    /** Is the specified path a package directory?
     * @param p Path.
     * @returns A boolean value.
     */
    bool is_pkg_dir(const std::string &p);

    /** Are we inside a package directory?
     * @returns A boolean value.
     */
    inline bool in_pkg_dir() { return is_pkg_dir(util::getcwd()); }

    /** Is the specified path an ebuild?
     * @param p Path.
     * @returns A boolean value.
     */
    inline bool is_ebuild(const std::string &path)
    {
        return ( (path.length() > 7) and
                 (path.substr(path.length() - 7) == ".ebuild") );
    }

    /**
     * Represents a list of valid package categories.
     */

    class categories_T
    {
        public:
            typedef std::set<std::string> value_type;
            typedef value_type::iterator iterator;
            typedef value_type::const_iterator const_iterator;
            typedef value_type::size_type size_type;

            /** Constructor.
             * @param validate Whether or not to validate categories.
             */
            categories_T(bool validate = false)
                : _portdir(portage::portdir()), _validate(validate)
            { this->init(); }

            /** Constructor.
             * @param p        PORTDIR.
             * @param validate Whether or not to validate categories.
             */
            categories_T(const std::string &p, bool validate = false)
                : _portdir(p), _validate(validate)
            { this->init(); }

            /* set subset */
            iterator begin() { return this->_s.begin(); }
            const_iterator begin() const { return this->_s.begin(); }
            iterator end() { return this->_s.end(); }
            const_iterator end() const { return this->_s.end(); }
            bool find(const value_type::key_type &k) const
            { return (this->_s.find(k) != this->_s.end()); }
            size_type size() const { return this->_s.size(); }
            void clear() { return this->_s.clear(); }

        private:
            void init();

            /// PORTDIR.
            const std::string _portdir;
            /// validate categories?
            bool _validate;
            /// category std::string container
            static value_type _s;
            /// has _s been initialized?
            static bool _init;
    };
}

#endif

/* vim: set tw=80 sw=4 et : */
