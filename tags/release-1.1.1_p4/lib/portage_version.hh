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
#include <utility>
#include <algorithm>
#include <cstdlib>
#include <cassert>

#include "file.hh"

namespace portage
{
    /**
     * Represents a single version string.
     */

    class version_string_T
    {
        public:
            typedef util::string string_type;
            typedef std::map<string_type, string_type> value_type;
            typedef value_type::iterator iterator;
            typedef value_type::const_iterator const_iterator;

            /** Constructor.
             * @param path Path to ebuild.
             */
            version_string_T(const util::path_T &path) : _ebuild(path),
                _verstr(util::chop_fileext(path.basename()))
            { this->init(); }

//            const suffix_T &suffix() const
//            { return this->_suffix; }

            /** Prepare version string in a format identical to what
             * portage would use.
             * @returns String object.
             */
            const string_type operator() () const;

            /** Get version string (minus the suffix).
             * @returns String object.
             */
            const string_type &version() const { return this->_version(); }

            /** Get path to ebuild for this version.
             * @returns String object.
             */
            const util::path_T &ebuild() const { return this->_ebuild; }

            /** Determine whether given version_string_T object is less
             * than this one.
             * @param that Reference to a version_string_T object.
             * @returns    A boolean value.
             */
            bool operator< (version_string_T &that);

            /** Determine whether given version_string_T object is greater
             * than this one.
             * @param that Reference to a version_string_T object.
             * @returns    A boolean value.
             */
            bool operator> (version_string_T &that)
            { return !(*this < that); }

            /** Determine whether given version_string_T object is equal
             * to this one.
             * @param that Reference to a version_string_T object.
             * @returns    A boolean value.
             */
            bool operator==(version_string_T &that);

            /** Determine whether given version_string_T object is not equal
             * to this one.
             * @param that Reference to a version_string_T object.
             * @returns    A boolean value.
             */
            bool operator!=(version_string_T &that)
            { return !(*this == that); }

            /** Retrieve values for version components (ie. P, PV, PN, etc).
             * @param s String object.
             * @returns String object mapped to s.
             */
            const string_type &operator[] (const string_type &s)
            { return this->_v[s]; }
            
            /* map subset */
            iterator begin() { return this->_v.begin(); }
            const_iterator begin() const { return this->_v.begin(); }
            iterator end() { return this->_v.end(); }
            const_iterator end() const { return this->_v.end(); }
            iterator find(const string_type &s) { return this->_v.find(s); }
            const_iterator find(const string_type &s) const
            { return this->_v.find(s); }

        protected:
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
                     * @param pvr PVR string object (version+revision).
                     */
                    suffix_T(const string_type &pvr) { this->init(pvr); }

                    /** Get suffix string.
                     * @returns String object.
                     */
                    const string_type &suffix() const { return this->_suffix; }

                    /** Get suffix version string.
                     * @returns String object.
                     */
                    const string_type &version() const
                    { return this->_suffix_ver; }

                    /** Assign a new PVR string.
                     * @param pvr PVR string object (version+revision).
                     */
                    void assign(const string_type &pvr) { this->init(pvr); }

                    /** Determine whether that suffix is less than this suffix.
                     * @param that Reference to a suffix_T object.
                     * @returns    A boolean value.
                     */
                    bool operator< (suffix_T &that);

                    /** Determine whether that suffix is greater than this
                     * suffix.
                     * @param that Reference to a suffix_T object.
                     * @returns    A boolean value.
                     */
                    bool operator> (suffix_T &that)
                    { return not (*this < that); }
                    
                    /** Determine whether that suffix is equal to this suffix.
                     * @param that Reference to a suffix_T object.
                     * @returns    A boolean value.
                     */
                    bool operator==(suffix_T &that);
                    
                    /** Determine whether that suffix is not equal to this
                     * suffix.
                     * @param that Reference to a suffix_T object.
                     * @returns    A boolean value.
                     */
                    bool operator!=(suffix_T &that)
                    { return not (*this == that); }

                protected:
                    /// Initialize this suffix string.
                    void init(const string_type &);
                    /// Parse PVR string to get suffix string.
                    void get_suffix(const string_type &);

                    /// Valid suffixes.
                    static std::vector<string_type> _suffixes;
                    /// Suffix string.
                    string_type _suffix;
                    /// Suffix version string.
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
                     * @param pv PV string object.
                     */
                    nosuffix_T(const string_type &pv) { this->init(pv); }

                    /** Get version string minus suffix.
                     * @returns String object.
                     */
                    const string_type &operator() () const
                    { return this->_version; }

                    /** Assign new PV string.
                     * @param pv PV string object.
                     */
                    void assign(const string_type &pv) { this->init(pv); }

                    /** Determine whether that nosuffix is less than this
                     * nosuffix.
                     * @param that Reference to a nosuffix_T object.
                     * @returns    A boolean value.
                     */
                    bool operator< (nosuffix_T &that);

                    /** Determine whether that nosuffix is greater than this
                     * nosuffix.
                     * @param that Reference to a nosuffix_T object.
                     * @returns    A boolean value.
                     */
                    bool operator> (nosuffix_T &that)
                    { return !(*this < that); }

                    /** Determine whether that nosuffix is equal to this
                     * nosuffix.
                     * @param that Reference to a nosuffix_T object.
                     * @returns    A boolean value.
                     */
                    bool operator==(nosuffix_T &that);

                    /** Determine whether that nosuffix is not equal to this
                     * nosuffix.
                     * @param that Reference to a nosuffix_T object.
                     * @returns    A boolean value.
                     */
                    bool operator!=(nosuffix_T &that)
                    { return !(*this == that); }

                protected:
                    /// Initialize this nosuffix string.
                    void init(const string_type &);
                    /// Version string (minus suffix).
                    string_type _version;
                    /// Any extra non-digit characters.
                    string_type _extra;
            };

            /// Initialize this version string.
            void init();
            /// Parse this version string and fill version components map.
            void parse();

            /// Absolute path to ebuild.
            const util::path_T _ebuild;
            /// Full version string.
            string_type _verstr;
            /// Version components map.
            value_type  _v;
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
            versions_T(const util::path_T &path) { this->assign(path); }

            /** Constructor.  Instantiate version_string_T objects for each
             * ebuild existing in each element (package directories).
             * @param v Vector of package directory paths.
             */
            versions_T(const std::vector<util::path_T> &v);

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
            virtual bool insert(const util::path_T &p);

            /** Assign a new package directory clearing any previously
             * contained version_string_T instances.
             * @param p Path to package directory.
             */
            virtual void assign(const util::path_T &p);

            /** Append a new package directory.
             * @param p Path to package directory.
             */
            virtual void append(const util::path_T &p);

        protected:
            /// version_string_T container
            value_type _vs;
    };
}

#endif

/* vim: set tw=80 sw=4 et : */
