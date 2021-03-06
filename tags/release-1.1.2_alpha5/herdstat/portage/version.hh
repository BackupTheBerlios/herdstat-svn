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

#include <herdstat/util/functional.hh>
#include <herdstat/util/file.hh>

namespace portage
{
    /**
     * Version component (${P}, ${PN}, etc) map.
     */

    class version_map
    {
        public:
            typedef std::map<std::string, std::string> container_type;
            typedef container_type::value_type value_type;
            typedef container_type::iterator iterator;
            typedef container_type::const_iterator const_iterator;
            typedef container_type::size_type size_type;

            version_map(const std::string &path);

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
            /// Parse version string and insert components into map.
            void parse();

            std::string _verstr;
            mutable container_type _vmap;
    };

    /**
     * Represents a single version string.
     */

    class version_string
    {
        public:
            /** Constructor.
             * @param path Path to ebuild.
             */
            version_string(const std::string &path);

            /// Implicit conversion to std::string.
            operator std::string() const;

            /** Get version string (minus the suffix).
             * @returns String object.
             */
            const std::string &version() const { return this->_version(); }

            /** Get path to ebuild for this version.
             * @returns String object.
             */
            const std::string &ebuild() const { return this->_ebuild; }

            /** Get version component map for this version string.
             * @returns Reference to version_map object.
             */
            const version_map& components() const { return this->_v; }

            /** Determine whether given version_string object is less
             * than this one.
             * @param that Reference to a version_string object.
             * @returns    A boolean value.
             */
            bool operator< (const version_string& that) const;

            /** Determine whether given version_string object is greater
             * than this one.
             * @param that Reference to a version_string object.
             * @returns    A boolean value.
             */
            bool operator> (const version_string& that) const
            { return (that < *this); }

            /** Determine whether given version_string object is equal
             * to this one.
             * @param that Reference to a version_string object.
             * @returns    A boolean value.
             */
            bool operator==(const version_string& that) const;

            /** Determine whether given version_string object is not equal
             * to this one.
             * @param that Reference to a version_string object.
             * @returns    A boolean value.
             */
            bool operator!=(const version_string& that) const
            { return not (*this == that); }

        private:
            /**
             * Represents a version suffix (_alpha, _beta, etc).
             */

            class suffix
            {
                public:
                    /// Default constructor.
                    suffix();

                    /** Constructor.
                     * @param pvr PVR std::string object (version+revision).
                     */
                    suffix(const std::string &pvr);

                    /** Get suffix string.
                     * @returns String object.
                     */
                    const std::string& str() const { return this->_suffix; }

                    /** Get suffix version string.
                     * @returns String object.
                     */
                    const std::string& version() const
                    { return this->_suffix_ver; }

                    /** Determine whether that suffix is less than this suffix.
                     * @param that Reference to a suffix object.
                     * @returns    A boolean value.
                     */
                    bool operator< (const suffix& that) const;

                    /** Determine whether that suffix is greater than this
                     * suffix.
                     * @param that Reference to a suffix object.
                     * @returns    A boolean value.
                     */
                    bool operator> (const suffix& that) const
                    { return (that < *this); }
                    
                    /** Determine whether that suffix is equal to this suffix.
                     * @param that Reference to a suffix object.
                     * @returns    A boolean value.
                     */
                    bool operator== (const suffix& that) const;
                    
                    /** Determine whether that suffix is not equal to this
                     * suffix.
                     * @param that Reference to a suffix object.
                     * @returns    A boolean value.
                     */
                    bool operator!= (const suffix& that) const
                    { return not (*this == that); }

                private:
                    /// Parse ${PVR}
                    void parse(const std::string &pvr) const;

                    /// Valid suffixes.
                    static std::vector<std::string> _suffixes;
                    /// Suffix std::string.
                    mutable std::string _suffix;
                    /// Suffix version std::string.
                    mutable std::string _suffix_ver;
            };

            /**
             * Represents package version minus the suffix.
             */

            class nosuffix
            {
                public:
                    nosuffix();

                    /** Constructor.
                     * @param pv PV std::string object.
                     */
                    nosuffix(const std::string &pv);

                    /** Get version string minus suffix.
                     * @returns String object.
                     */
                    const std::string& operator() () const
                    { return this->_version; }

                    /** Determine whether that nosuffix is less than this
                     * nosuffix.
                     * @param that Reference to a nosuffix object.
                     * @returns    A boolean value.
                     */
                    bool operator< (const nosuffix& that) const;

                    /** Determine whether that nosuffix is greater than this
                     * nosuffix.
                     * @param that Reference to a nosuffix object.
                     * @returns    A boolean value.
                     */
                    bool operator> (const nosuffix& that) const
                    { return (that < *this); }

                    /** Determine whether that nosuffix is equal to this
                     * nosuffix.
                     * @param that Reference to a nosuffix object.
                     * @returns    A boolean value.
                     */
                    bool operator== (const nosuffix& that) const;

                    /** Determine whether that nosuffix is not equal to this
                     * nosuffix.
                     * @param that Reference to a nosuffix object.
                     * @returns    A boolean value.
                     */
                    bool operator!= (const nosuffix& that) const
                    { return not (*this == that); }

                private:
                    /// Parse ${PV}.
                    void parse(const std::string& pv) const;

                    /// Version string (minus suffix).
                    mutable std::string _version;
                    /// Any extra non-digit characters.
                    mutable std::string _extra;
            };

            /// Absolute path to ebuild.
            const std::string _ebuild;
            /// Version components map.
            const version_map  _v;
            /// Reference to actual version string
            const std::string &_verstr;
            /// Our version suffix.
            const suffix _suffix;
            /// Our version minus suffix.
            const nosuffix _version;
    };

    /** 
     * version_string container - generally used
     * for all versions of a single package.
     */

    class versions
    {
        public:
            typedef std::set<version_string *,
                util::DereferenceLess<version_string> > container_type;
            typedef container_type::iterator iterator;
            typedef container_type::const_iterator const_iterator;
            typedef container_type::size_type size_type;
            typedef container_type::value_type value_type;
            typedef container_type::reference reference;
            typedef container_type::const_reference const_reference;

            /// Default constructor.
            versions();

            /** Constructor.  Instantiate version_string objects for each
             * ebuild existing in the specified package directory.
             * @param path Path to package directory.
             */
            versions(const std::string &path);

            /** Constructor.  Instantiate version_string objects for each
             * ebuild existing in each element (package directories).
             * @param v Vector of package directory paths.
             */
            versions(const std::vector<std::string> &v);

            /// Destructor.
            ~versions();

            /* small set subset */
            iterator begin() { return _vs.begin(); }
            const_iterator begin() const { return _vs.begin(); }
            iterator end() { return _vs.end(); }
            const_iterator end() const { return _vs.end(); }
            iterator find(const std::string&);
            const_iterator find(const std::string&) const;
            size_type size() const { return _vs.size(); }
            bool empty() const { return _vs.empty(); }
            void clear() { _vs.clear(); }

            inline version_string *front();
            inline const version_string * const front() const;
            inline version_string *back();
            inline const version_string * const back() const;

            /** Instantiate and insert a version_string object with the
             * specified path.
             * @param p Path.
             * @returns A boolean value (whether insertion succeeded).
             */
            bool insert(const std::string &p);

            /** Assign a new package directory clearing any previously
             * contained version_string instances.
             * @param p Path to package directory.
             */
            void assign(const std::string &p);

            /** Append a new package directory.
             * @param p Path to package directory.
             */
            void append(const std::string &p);

        private:
            /// version_string container
            mutable container_type _vs;
    };

    inline version_string *
    versions::front()
    {
        assert(not this->empty());
        return *(this->begin());
    }

    inline const version_string * const
    versions::front() const
    {
        assert(not this->empty());
        return *(this->begin());
    }

    inline version_string *
    versions::back()
    {
        assert(not this->empty());
        iterator i = this->end();
        return *(--i);
    }

    inline const version_string * const
    versions::back() const
    {
        assert(not this->empty());
        const_iterator i = this->end();
        return *(--i);
    }
}

#endif

/* vim: set tw=80 sw=4 et : */
