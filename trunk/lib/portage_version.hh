/*
 * herdstat -- lib/portage_version.hh
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

#ifndef HAVE_PORTAGE_VERSION_HH
#define HAVE_PORTAGE_VERSION_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <set>
#include <map>
#include <vector>
#include <string>
#include <utility>
#include <algorithm>

#include "file.hh"

namespace portage
{
    /* Represents a version suffix (_alpha, _beta, etc) */
    class version_suffix_T
    {
        protected:
            void get_suffix(const std::string &);

            static std::vector<std::string> _suffices; /* valid suffices */
            std::string _suffix;        /* suffix */
            std::string _suffix_ver;    /* suffix version */

        public:
            version_suffix_T() { }
            version_suffix_T(const char *s) { this->init(s); }
            version_suffix_T(const std::string &s) { this->init(s); }

            void init(const std::string &);
            const std::string &suffix() const { return _suffix; }
            const std::string &version() const { return _suffix_ver; }

            void operator= (std::string &s) { this->init(s); }
            bool operator< (version_suffix_T &);
            bool operator> (version_suffix_T &s) { return !(*this < s); }
            bool operator==(version_suffix_T &);
            bool operator!=(version_suffix_T &s) { return !(*this == s); }
    };

    /* Represents a single version string */
    class version_string_T
    {
        protected:
            void init();
            void split_verstr();

            const std::string _ebuild;             /* abs path of ebuild */
            std::string _verstr;                   /* full version string */
            std::map<std::string, std::string> _v; /* version component map */
            portage::version_suffix_T _suffix;

        public:
            version_string_T(const char *path) : _ebuild(path),
                _verstr(util::chop_fileext(util::basename(path)))
            { this->init(); }
            version_string_T(const std::string &path) : _ebuild(path),
                _verstr(util::chop_fileext(util::basename(path)))
            { this->init(); }

            const portage::version_suffix_T &suffix() const { return _suffix; }

            const std::string operator() () const;
            const std::string &operator[] (const std::string &s)
            { return _v[s]; }
            const std::string &operator[] (const char *s) { return _v[s]; }

            bool operator< (version_string_T &);
            bool operator> (version_string_T &v) { return !(*this < v); }
            bool operator==(version_string_T &);
            bool operator!=(version_string_T &v) { return !(*this == v); }
    };

    /* version_string_T sorting criterion */
    class version_sort_T
    {
        public:
            bool operator() (portage::version_string_T *v1,
                             portage::version_string_T *v2)
            { return *v1 < *v2; }
    };

    /* 
     * version_string_T container - generally used
     * for all versions of a single package.
     */

    class versions_T
    {
        protected:
            std::set<portage::version_string_T *, version_sort_T> _vs;

        public:
            typedef std::set<portage::version_string_T *,
                             version_sort_T>::iterator iterator;
            typedef std::set<portage::version_string_T *,
                             version_sort_T>::size_type size_type;

            /* small set subset */
            iterator begin() { return _vs.begin(); }
            iterator end() { return _vs.end(); }
            size_type size() const { return _vs.size(); }
            std::pair<iterator, bool> insert(portage::version_string_T *s)
            { return _vs.insert(s); }
    };
}

#endif

/* vim: set tw=80 sw=4 et : */
