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

#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <utility>
#include <algorithm>
#include <cstdlib>
#include <cassert>

#include "file.hh"

namespace portage
{
    /* Represents a version suffix (_alpha, _beta, etc) */
    class version_suffix_T
    {
        public:
            typedef util::string string_type;
            typedef std::vector<string_type> value_type;

            version_suffix_T() { }
            version_suffix_T(const char *pvr) { this->init(pvr); }
            version_suffix_T(const string_type &pvr) { this->init(pvr); }

            const string_type &suffix() const { return this->_suffix; }
            const string_type &version() const { return this->_suffix_ver; }

            void assign(const string_type &pvr) { this->init(pvr); }

            bool operator< (version_suffix_T &);
            bool operator> (version_suffix_T &s) { return not (*this < s); }
            bool operator==(version_suffix_T &);
            bool operator!=(version_suffix_T &s) { return not (*this == s); }

        protected:
            void init(const string_type &);
            void get_suffix(const string_type &);

            static value_type _suffixes; /* valid suffixes */
            string_type _suffix;        /* suffix */
            string_type _suffix_ver;    /* suffix version */
    };

    /* represents ${PV} minus the suffix */
    class version_nosuffix_T
    {
        public:
            typedef util::string string_type;

            version_nosuffix_T() { }
            version_nosuffix_T(const char *pv) { this->init(pv); }
            version_nosuffix_T(const string_type &pv) { this->init(pv); }

            const string_type &operator() () const { return this->_version; }

            void assign(const string_type &pv) { this->init(pv); }

            bool operator< (version_nosuffix_T &);
            bool operator> (version_nosuffix_T &s) { return !(*this < s); }
            bool operator==(version_nosuffix_T &);
            bool operator!=(version_nosuffix_T &s) { return !(*this == s); }

        protected:
            void init(const string_type &);
            string_type _version;
    };

    /* Represents a single version string */
    class version_string_T
    {
        public:
            typedef util::string string_type;
            typedef std::map<string_type, string_type> value_type;
            typedef value_type::iterator iterator;
            typedef value_type::const_iterator const_iterator;

            version_string_T(const util::path_T &path) : _ebuild(path),
                _verstr(util::chop_fileext(path.basename()))
            { this->init(); }

            const portage::version_suffix_T &suffix() const
            { return this->_suffix; }

            const string_type operator() () const;
            const string_type &version() const { return this->_version(); }
            const util::path_T &ebuild() const { return this->_ebuild; }

            bool operator< (version_string_T &);
            bool operator> (version_string_T &v) { return !(*this < v); }
            bool operator==(version_string_T &);
            bool operator!=(version_string_T &v) { return !(*this == v); }

            /* map subset for accessing P, PN, PV, etc */
            const string_type &operator[] (const string_type &s)
            { return this->_v[s]; }
            
            iterator begin() { return this->_v.begin(); }
            const_iterator begin() const { return this->_v.begin(); }
            iterator end() { return this->_v.end(); }
            const_iterator end() const { return this->_v.end(); }
            iterator find(const string_type &s) { return this->_v.find(s); }
            const_iterator find(const string_type &s) const
            { return this->_v.find(s); }

        protected:
            void init();
            void parse();

            const util::path_T _ebuild;             /* abs path of ebuild */
            string_type _verstr;                    /* full version string */
            value_type  _v;                         /* version component map */
            portage::version_suffix_T _suffix;      /* version suffix object */
            portage::version_nosuffix_T _version;   /* version minus suffix */
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
        public:
            typedef std::set<portage::version_string_T *,
                             portage::version_sort_T> value_type;
            typedef value_type::iterator iterator;
            typedef value_type::const_iterator const_iterator;
            typedef value_type::size_type size_type;
            typedef portage::version_string_T::string_type string_type;

            versions_T() { }
            versions_T(const util::path_T &path) { this->assign(path); }
            virtual ~versions_T();

            /* small set subset */
            iterator begin() { return this->_vs.begin(); }
            const_iterator begin() const { return this->_vs.begin(); }
            iterator end() { return this->_vs.end(); }
            const_iterator end() const { return this->_vs.end(); }
            iterator find(const string_type &);
            size_type size() const { return this->_vs.size(); }
            bool empty() const { return this->size() == 0; }

            portage::version_string_T *front() { return *(++this->begin()); }
            portage::version_string_T *back() { return *(--this->end()); }

            virtual bool insert(const util::path_T &);
            virtual void assign(const util::path_T &);

        protected:
            value_type _vs;
    };
}

#endif

/* vim: set tw=80 sw=4 et : */
