/*
 * herdstat -- portage/herd.hh
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

#ifndef _HAVE_HERD_HH
#define _HAVE_HERD_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/**
 * @file herd.hh
 * @brief Defines the herd and herds classes.
 */

#include <herdstat/portage/developer.hh>

namespace portage {

    /**
     * Represents a herd.
     * Techically a herd is a group of packages, however here it makes more
     * sense to represent them as a group of developers.
     */

    class Herd : public Developers
    {
        public:
            Herd();
            Herd(const std::string &name,
                 const std::string &email = "",
                 const std::string &desc  = "");
            Herd(const Herd& that);
            Herd(const container_type& v);
            Herd(const std::vector<std::string>& v);

            virtual ~Herd();

            Herd& operator= (const Herd& that);

            /// Implicit conversion to std::string.
            operator std::string() const;

            /** Determine if this herd is equal to that herd.
             * @param name herd name.
             * @returns True if herd names are equivelent.
             */
            bool operator== (const std::string& name) const;
            bool operator== (const Herd& herd) const;
            bool operator== (const util::regex_T& re) const;
            bool operator!= (const std::string& name) const;
            bool operator!= (const Herd& herd) const;
            bool operator!= (const util::regex_T& re) const;
            bool operator<  (const std::string& name) const;
            bool operator<  (const Herd& herd) const;
            bool operator>  (const std::string& name) const;
            bool operator>  (const Herd& herd) const;
            bool operator<= (const std::string& name) const;
            bool operator<= (const Herd& herd) const;
            bool operator>= (const std::string& name) const;
            bool operator>= (const Herd& herd) const;

            const std::string& name() const;
            const std::string& email() const;
            const std::string& desc() const;
            void set_name(const std::string &name);
            void set_email(const std::string &email);
            void set_desc(const std::string &desc);

        private:
            std::string _name;
            GentooEmailAddress _email;
            std::string _desc;
    };

    inline Herd::operator std::string() const { return _name; }
    inline bool Herd::operator== (const std::string& name) const
    { return (_name == name); }
    inline bool Herd::operator== (const Herd& herd) const
    { return (_name == herd._name); }
    inline bool Herd::operator== (const util::regex_T& re) const
    { return (re == _name); }
    inline bool Herd::operator!= (const std::string& name) const
    { return (_name != name); }
    inline bool Herd::operator!= (const Herd& herd) const
    { return (_name != herd._name); }
    inline bool Herd::operator!= (const util::regex_T& re) const
    { return (re != _name); }
    inline bool Herd::operator< (const std::string& name) const
    { return (_name < name); }
    inline bool Herd::operator< (const Herd& herd) const
    { return (_name < herd._name); }
    inline bool Herd::operator> (const std::string& name) const
    { return (_name > name); }
    inline bool Herd::operator> (const Herd& herd) const
    { return (_name > herd._name); }
    inline bool Herd::operator<= (const std::string& name) const
    { return (_name <= name); }
    inline bool Herd::operator<= (const Herd& herd) const
    { return (_name <= herd._name); }
    inline bool Herd::operator>= (const std::string& name) const
    { return (_name >= name); }
    inline bool Herd::operator>= (const Herd& herd) const
    { return (_name >= herd._name); }
    inline const std::string& Herd::name() const { return _name; }
    inline const std::string& Herd::email() const { return _email.str(); }
    inline const std::string& Herd::desc() const { return _desc; }
    inline void Herd::set_name(const std::string& name) { _name.assign(name); }
    inline void Herd::set_email(const std::string& email) { _email.assign(email); }
    inline void Herd::set_desc(const std::string& desc) { _desc.assign(desc); }

    /**
     * Herd container.
     */

    class Herds
    {
        public:
            typedef std::vector<Herd *> container_type;
            typedef container_type::value_type value_type;
            typedef container_type::reference reference;
            typedef container_type::const_reference const_reference;
            typedef container_type::iterator iterator;
            typedef container_type::const_iterator const_iterator;
            typedef container_type::size_type size_type;

            /// Default constructor.
            Herds();

            /** Constructor.  Initialize this herd with a container_type.
             * @param v Reference to a container_type.
             */
            Herds(const container_type& v);

            /** Copy constructor.
             * @param that Reference to another Herds object.
             */
            Herds(const Herds& that);

            /// Destructor.
            ~Herds();

            /// Implicit conversion to container_type.
            operator container_type() const;
            /// Implicit conversion to std::vector<std::string>.
            operator std::vector<std::string>() const;

            /** Assign a new container_type.
             * @param v Reference to a container_type object.
             * @returns Reference to this.
             */
            Herds& operator= (const container_type& v);

            /** Instantiate a Herd for each string in the given vector and
             * insert it into our container.
             * @param v Reference to a std::vector<std::string> of herd names.
             * @returns Reference to this.
             */
            Herds& operator= (const std::vector<std::string>& v);

            /** Copy assignment operator.
             * @param that Reference to another Herds object.
             * @returns Reference to this.
             */
            Herds& operator= (const Herds& that);

            /* container_type subset */
            iterator begin();
            const_iterator begin() const;
            iterator end();
            const_iterator end() const;
            reference front();
            const_reference front() const;
            reference back();
            const_reference back() const;
            size_type size() const;
            bool empty() const;
            void clear();
            void push_back(const value_type h);
            iterator insert(iterator pos, const value_type h);
            template <class In>
            void insert(iterator pos, In begin, In end);

            iterator find(const std::string &herd);
            iterator find(const value_type h);
            iterator find(const util::regex_T &regex);
            const_iterator find(const std::string &herd) const;
            const_iterator find(const value_type h) const;
            const_iterator find(const util::regex_T &regex) const;

        private:
            container_type _herds;
    };

    inline Herds::operator Herds::container_type() const { return _herds; }
    inline Herds& Herds::operator= (const Herds& that)
    { *this = that._herds; return *this; }
    inline Herds::iterator Herds::begin() { return _herds.begin(); }
    inline Herds::const_iterator Herds::begin() const { return _herds.begin(); }
    inline Herds::iterator Herds::end() { return _herds.end(); }
    inline Herds::const_iterator Herds::end() const { return _herds.end(); }
    inline Herds::reference Herds::front() { return _herds.front(); }
    inline Herds::const_reference Herds::front() const { return _herds.front(); }
    inline Herds::reference Herds::back() { return _herds.back(); }
    inline Herds::const_reference Herds::back() const { return _herds.back(); }

    inline Herds::size_type Herds::size() const { return _herds.size(); }
    inline bool Herds::empty() const { return _herds.empty(); }
    inline void Herds::clear() { return _herds.clear(); }
    inline void Herds::push_back(const value_type h) { _herds.push_back(h); }
    inline Herds::iterator Herds::insert(iterator pos, const value_type h)
    { return _herds.insert(pos, h); }
    template <class In> inline void
    Herds::insert(iterator pos, In begin, In end)
    { _herds.insert(pos, begin, end); }

    inline Herds::iterator Herds::find(const std::string& herd)
    { return std::find_if(_herds.begin(), _herds.end(), std::bind2nd(
        util::DereferenceStrEqual<Herd>(), herd)); }
    inline Herds::const_iterator Herds::find(const std::string& herd) const
    { return std::find_if(_herds.begin(), _herds.end(), std::bind2nd(
        util::DereferenceStrEqual<Herd>(), herd)); }
    inline Herds::iterator Herds::find(const value_type h)
    { return std::find_if(_herds.begin(), _herds.end(), std::bind2nd(
        util::DereferenceEqual<Herd>(), h)); }
    inline Herds::const_iterator Herds::find(const value_type h) const
    { return std::find_if(_herds.begin(), _herds.end(), std::bind2nd(
        util::DereferenceEqual<Herd>(), h)); }

    struct HerdRegexMatch
        : std::binary_function<const util::regex_T *, const Herd *, bool>
    {
        bool operator() (const util::regex_T *re, const Herd *herd) const
        { return (*re == herd->name()); }
    };

    inline Herds::iterator Herds::find(const util::regex_T &regex)
    { return std::find_if(_herds.begin(), _herds.end(),
            std::bind1st(HerdRegexMatch(), &regex)); }
    inline Herds::const_iterator Herds::find(const util::regex_T &regex) const
    { return std::find_if(_herds.begin(), _herds.end(),
            std::bind1st(HerdRegexMatch(), &regex)); }

} // namespace portage

#endif /* _HAVE_HERD_H */

/* vim: set tw=80 sw=4 et : */
