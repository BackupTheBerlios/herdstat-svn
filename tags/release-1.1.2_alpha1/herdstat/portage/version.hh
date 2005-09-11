/*
 * herdstat -- herdstat/portage/portage_version.hh
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

#include <string>
#include <set>
#include <map>
#include <vector>
#include <utility>
#include <algorithm>
#include <cstdlib>
#include <cassert>

#include <herdstat/util/file.hh>

namespace portage
{
    /**
     * Version component (${P}, ${PN}, etc) map.
     */

    class version_map_T
    {
        public:
            typedef std::map<std::string, std::string> container_type;
            typedef container_type::iterator iterator;
            typedef container_type::const_iterator const_iterator;
            typedef container_type::size_type size_type;

            version_map_T(const std::string &path);

            /* map subset */
            iterator begin() { return _vmap.begin(); }
            const_iterator begin() const { return _vmap.begin(); }
            iterator end() { return _vmap.end(); }
            const_iterator end() const { return _vmap.end(); }
            iterator find(const std::string &s) { return _vmap.find(s); }
            const_iterator find(const std::string &s) const
            { return _vmap.find(s); }

            const std::string &operator[](const std::string &key)
            { return _vmap[key]; }
            const std::string operator[](const std::string &key) const
            {
                const_iterator i = this->find(key);
                return (i == this->end() ? "" : i->second);
            }

            const std::string& version() const { return _verstr; }

        private:
            void parse_verstr();

            std::string _verstr;
            mutable container_type _vmap;
    };

    /**
     * Represents a single version string.
     */

    class version_string_T
    {
        public:
            typedef std::string string_type;
            typedef std::map<string_type, string_type> value_type;
            typedef value_type::iterator iterator;
            typedef value_type::const_iterator const_iterator;

            /** Constructor.
             * @param path Path to ebuild.
             */
            version_string_T(const std::string &path);

            /// Implicit conversion to std::string.
            operator std::string() const;

            /** Get version std::string (minus the suffix).
             * @returns String object.
             */
            const string_type &version() const { return this->_version(); }

            /** Get path to ebuild for this version.
             * @returns String object.
             */
            const std::string &ebuild() const { return this->_ebuild; }

            /** Get version component map for this version string.
             * @returns Reference to version_map_T object.
             */
            const version_map_T& components() const { return this->_v; }

            /** Determine whether given version_string_T object is less
             * than this one.
             * @param that Reference to a version_string_T object.
             * @returns    A boolean value.
             */
            bool operator< (version_string_T &that) const;

            /** Determine whether given version_string_T object is greater
             * than this one.
             * @param that Reference to a version_string_T object.
             * @returns    A boolean value.
             */
            bool operator> (version_string_T &that) const
            { return !(*this < that); }

            /** Determine whether given version_string_T object is equal
             * to this one.
             * @param that Reference to a version_string_T object.
             * @returns    A boolean value.
             */
            bool operator==(version_string_T &that) const;

            /** Determine whether given version_string_T object is not equal
             * to this one.
             * @param that Reference to a version_string_T object.
             * @returns    A boolean value.
             */
            bool operator!=(version_string_T &that) const
            { return !(*this == that); }

        private:
            /**
             * Represents a version suffix (_alpha, _beta, etc).
             */

            class suffix_T
            {
                public:
                    /// Default constructor.
                    suffix_T() { }

                    /** Constructor.
                     * @param pvr PVR char array (version+revision).
                     */
                    suffix_T(const char *pvr) { this->init(pvr); }

                    /** Constructor.
                     * @param pvr PVR std::string object (version+revision).
                     */
                    suffix_T(const string_type &pvr) { this->init(pvr); }

                    /** Get suffix std::string.
                     * @returns String object.
                     */
                    const string_type &suffix() const { return this->_suffix; }

                    /** Get suffix version std::string.
                     * @returns String object.
                     */
                    const string_type &version() const
                    { return this->_suffix_ver; }

                    /** Assign a new PVR std::string.
                     * @param pvr PVR std::string object (version+revision).
                     */
                    void assign(const string_type &pvr) { this->init(pvr); }

                    /** Determine whether that suffix is less than this suffix.
                     * @param that Reference to a suffix_T object.
                     * @returns    A boolean value.
                     */
                    bool operator< (suffix_T &that) const;

                    /** Determine whether that suffix is greater than this
                     * suffix.
                     * @param that Reference to a suffix_T object.
                     * @returns    A boolean value.
                     */
                    bool operator> (suffix_T &that) const
                    { return not (*this < that); }
                    
                    /** Determine whether that suffix is equal to this suffix.
                     * @param that Reference to a suffix_T object.
                     * @returns    A boolean value.
                     */
                    bool operator==(suffix_T &that) const;
                    
                    /** Determine whether that suffix is not equal to this
                     * suffix.
                     * @param that Reference to a suffix_T object.
                     * @returns    A boolean value.
                     */
                    bool operator!=(suffix_T &that) const
                    { return not (*this == that); }

                private:
                    /// Initialize this suffix std::string.
                    void init(const string_type &);
                    /// Parse PVR std::string to get suffix std::string.
                    void get_suffix(const string_type &);

                    /// Valid suffixes.
                    static std::vector<string_type> _suffixes;
                    /// Suffix std::string.
                    string_type _suffix;
                    /// Suffix version std::string.
                    string_type _suffix_ver;
            };

            /**
             * Represents package version minus the suffix.
             */

            class nosuffix_T
            {
                public:
                    nosuffix_T() { }

                    /** Constructor.
                     * @param pv PV char array.
                     */
                    nosuffix_T(const char *pv) { this->init(pv); }

                    /** Constructor.
                     * @param pv PV std::string object.
                     */
                    nosuffix_T(const string_type &pv) { this->init(pv); }

                    /** Get version std::string minus suffix.
                     * @returns String object.
                     */
                    const string_type &operator() () const
                    { return this->_version; }

                    /** Assign new PV std::string.
                     * @param pv PV std::string object.
                     */
                    void assign(const string_type &pv) { this->init(pv); }

                    /** Determine whether that nosuffix is less than this
                     * nosuffix.
                     * @param that Reference to a nosuffix_T object.
                     * @returns    A boolean value.
                     */
                    bool operator< (nosuffix_T &that) const;

                    /** Determine whether that nosuffix is greater than this
                     * nosuffix.
                     * @param that Reference to a nosuffix_T object.
                     * @returns    A boolean value.
                     */
                    bool operator> (nosuffix_T &that) const
                    { return !(*this < that); }

                    /** Determine whether that nosuffix is equal to this
                     * nosuffix.
                     * @param that Reference to a nosuffix_T object.
                     * @returns    A boolean value.
                     */
                    bool operator==(nosuffix_T &that) const;

                    /** Determine whether that nosuffix is not equal to this
                     * nosuffix.
                     * @param that Reference to a nosuffix_T object.
                     * @returns    A boolean value.
                     */
                    bool operator!=(nosuffix_T &that) const
                    { return !(*this == that); }

                private:
                    /// Initialize this nosuffix std::string.
                    void init(const string_type &);
                    /// Version std::string (minus suffix).
                    string_type _version;
                    /// Any extra non-digit characters.
                    string_type _extra;
            };

            /// Absolute path to ebuild.
            const std::string _ebuild;
            /// Version components map.
            const version_map_T  _v;
            /// Reference to actual version string
            const std::string &_verstr;
            /// Our version suffix.
            suffix_T _suffix;
            /// Our version minus suffix.
            nosuffix_T _version;
    };

    /**
     * Functor for sorting version_string_T objects with standard algorithms.
     */

    class version_sort_T
    {
        public:
            /** Is the first version_string_T less than the second?
             * @param v1 A pointer of type version_string_T.
             * @param v2 A pointer of type version_string_T.
             * @returns  A boolean value.
             */
            bool operator() (version_string_T *v1,
                             version_string_T *v2)
            { return *v1 < *v2; }
    };

    /** 
     * version_string_T container - generally used
     * for all versions of a single package.
     */

    class versions_T
    {
        public:
            typedef std::set<version_string_T *, version_sort_T> value_type;
            typedef value_type::iterator iterator;
            typedef value_type::const_iterator const_iterator;
            typedef value_type::size_type size_type;
            typedef version_string_T::string_type string_type;

            /// Default constructor.
            versions_T() { }

            /** Constructor.  Instantiate version_string_T objects for each
             * ebuild existing in the specified package directory.
             * @param path Path to package directory.
             */
            versions_T(const std::string &path) { this->assign(path); }

            /** Constructor.  Instantiate version_string_T objects for each
             * ebuild existing in each element (package directories).
             * @param v Vector of package directory paths.
             */
            versions_T(const std::vector<std::string> &v);

            virtual ~versions_T();

            /* small set subset */
            iterator begin() { return this->_vs.begin(); }
            const_iterator begin() const { return this->_vs.begin(); }
            iterator end() { return this->_vs.end(); }
            const_iterator end() const { return this->_vs.end(); }
            iterator find(const string_type &);
            size_type size() const { return this->_vs.size(); }
            bool empty() const { return this->_vs.size() == 0; }
            void clear() { this->_vs.clear(); }

            version_string_T *front() { return *(++this->begin()); }
            version_string_T *back() { return *(--this->end()); }

            /** Instantiate and insert a version_string_T object with the
             * specified path.
             * @param p Path.
             * @returns A boolean value (whether insertion succeeded).
             */
            virtual bool insert(const std::string &p);

            /** Assign a new package directory clearing any previously
             * contained version_string_T instances.
             * @param p Path to package directory.
             */
            virtual void assign(const std::string &p);

            /** Append a new package directory.
             * @param p Path to package directory.
             */
            virtual void append(const std::string &p);

        protected:
            /// version_string_T container
            value_type _vs;
    };
}

#endif

/* vim: set tw=80 sw=4 et : */
