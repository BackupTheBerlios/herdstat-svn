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
            Herd() { }
            Herd(const std::string &name,
                 const std::string &email = "",
                 const std::string &desc  = "");
            Herd(const std::vector<std::string>& v);

            /// Implicit conversion to std::string.
            operator std::string() const;

            /** Determine if this herd is equal to that herd.
             * @param name herd name.
             * @returns True if herd names are equivelent.
             */
            bool operator== (const std::string& name) const;
            bool operator== (const Herd& herd) const;
            bool operator!= (const std::string& name) const;
            bool operator!= (const Herd& herd) const;

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
    inline bool Herd::operator!= (const std::string& name) const
    { return (_name != name); }
    inline bool Herd::operator!= (const Herd& herd) const
    { return (_name != herd._name); }
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
            typedef std::vector<Herd> container_type;
            typedef container_type::value_type value_type;
            typedef container_type::reference reference;
            typedef container_type::const_reference const_reference;
            typedef container_type::iterator iterator;
            typedef container_type::const_iterator const_iterator;
            typedef container_type::size_type size_type;

            Herds() : _herds() { }
            Herds(const std::vector<Herd>& v) : _herds(v) { }

            /// Implicit conversion to std::vector<herd>
            operator std::vector<Herd>() const;
            operator std::vector<std::string>() const;

            Herds& operator= (const std::vector<Herd>& v);
            Herds& operator= (const std::vector<std::string>& v);

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
            void push_back(const Herd& h);

            iterator find(const std::string &herd);
            iterator find(const Herd& h);
            iterator find(const util::regex_T &regex);
            const_iterator find(const std::string &herd) const;
            const_iterator find(const Herd& h) const;
            const_iterator find(const util::regex_T &regex) const;

            template <class In>
            void insert(iterator pos, In begin, In end);

//            const value_type& operator[] (size_type pos) const;

        private:
            container_type _herds;
    };

    inline Herds::operator std::vector<Herd>() const { return _herds; }
    inline Herds& Herds::operator= (const std::vector<Herd>& v)
    { _herds = v; return *this; }
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
    inline void Herds::push_back(const Herd& h) { _herds.push_back(h); }

    inline Herds::iterator Herds::find(const std::string& herd)
    { return std::find(_herds.begin(), _herds.end(), herd); }
    inline Herds::const_iterator Herds::find(const std::string& herd) const
    { return std::find(_herds.begin(), _herds.end(), herd); }
    inline Herds::iterator Herds::find(const Herd& h)
    { return std::find(_herds.begin(), _herds.end(), h); }
    inline Herds::const_iterator Herds::find(const Herd& h) const
    { return std::find(_herds.begin(), _herds.end(), h); }
    inline Herds::iterator Herds::find(const util::regex_T &regex)
    { return std::find_if(_herds.begin(), _herds.end(),
            std::bind1st(util::regexMatch(), &regex)); }
    inline Herds::const_iterator Herds::find(const util::regex_T &regex) const
    { return std::find_if(_herds.begin(), _herds.end(),
            std::bind1st(util::regexMatch(), &regex)); }

    template <class In> inline void
    Herds::insert(iterator pos, In begin, In end)
    { _herds.insert(pos, begin, end); }

//    const Herds::value_type& Herds::operator[] (size_type pos) const
//    { return _herds[pos]; }

} // namespace portage

#endif /* _HAVE_HERD_H */

/* vim: set tw=80 sw=4 et : */
