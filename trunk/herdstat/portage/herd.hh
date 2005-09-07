/*
 * herdstat -- portage/herd.hh
 * $Id: herd.hh 520 2005-09-05 11:59:58Z ka0ttic $
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

#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <herdstat/util/regex.hh>
#include <herdstat/portage/developer.hh>

namespace portage {

    /**
     * Represents a herd.
     * Techically a herd is a group of packages, however here it makes more
     * sense to represent them as a group of developers.  Also note, that we
     * often use this class as a Developer container, even if the collection
     * isn't considered a "herd".
     */

    class Herd
    {
        public:
            typedef std::vector<Developer> container_type;
            typedef container_type::iterator iterator;
            typedef container_type::const_iterator const_iterator;
            typedef container_type::value_type value_type;
            typedef container_type::size_type size_type;

            Herd() { }
            Herd(const std::string &name,
                 const std::string &email = "",
                 const std::string &desc  = "");

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

            iterator begin();
            const_iterator begin() const;
            iterator end();
            const_iterator end() const;
            iterator find(const std::string& dev);
            const_iterator find(const std::string& dev) const;
            iterator find(const Developer& dev);
            const_iterator find(const Developer& dev) const;
            iterator find(const util::regex_T &regex);
            const_iterator find(const util::regex_T &regex) const;
            size_type size() const;
            bool empty() const;
            void clear();
            void push_back(const Developer& dev);
            void push_back(const std::string& email);
            template <class In>
            void insert(iterator pos, In begin, In end);

        private:
            container_type _devs;
            std::string _name;
            std::string _email;
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
    inline const std::string& Herd::email() const { return _email; }
    inline const std::string& Herd::desc() const { return _desc; }
    inline void Herd::set_name(const std::string &name) { _name.assign(name); }
    inline void Herd::set_desc(const std::string &desc) { _desc.assign(desc); }
    inline Herd::iterator Herd::begin() { return _devs.begin(); }
    inline Herd::const_iterator Herd::begin() const { return _devs.begin(); }
    inline Herd::iterator Herd::end() { return _devs.end(); }
    inline Herd::const_iterator Herd::end() const { return _devs.end(); }
    inline Herd::size_type Herd::size() const { return _devs.size(); }
    inline bool Herd::empty() const { return _devs.empty(); }
    inline void Herd::clear() { return _devs.clear(); }
    inline void Herd::push_back(const Developer& dev) { _devs.push_back(dev); }

    inline Herd::iterator Herd::find(const std::string& dev)
    { return std::find(_devs.begin(), _devs.end(), dev.substr(0, dev.find('@'))); }
    inline Herd::const_iterator Herd::find(const std::string& dev) const
    { return std::find(_devs.begin(), _devs.end(), dev.substr(0, dev.find('@'))); }
    inline Herd::iterator Herd::find(const Developer& dev)
    { return std::find(_devs.begin(), _devs.end(), dev); }
    inline Herd::const_iterator Herd::find(const Developer& dev) const
    { return std::find(_devs.begin(), _devs.end(), dev); }
    inline Herd::iterator Herd::find(const util::regex_T &regex)
    { return std::find_if(_devs.begin(), _devs.end(),
            std::bind1st(util::regexMatch(), &regex)); }
    inline Herd::const_iterator Herd::find(const util::regex_T &regex) const
    { return std::find_if(_devs.begin(), _devs.end(),
            std::bind1st(util::regexMatch(), &regex)); }
    template <class In> inline void
    Herd::insert(iterator pos, In begin, In end)
    { _devs.insert(pos, begin, end); }


    /**
     * Herd container.
     */

    class Herds
    {
        public:
            typedef std::vector<Herd> container_type;
            typedef container_type::value_type value_type;
            typedef container_type::iterator iterator;
            typedef container_type::const_iterator const_iterator;
            typedef container_type::size_type size_type;

            Herds() : _herds() { }

            /// Implicit conversion to std::vector<herd>
            operator std::vector<Herd>() const;

            iterator begin();
            const_iterator begin() const;
            iterator end();
            const_iterator end() const;
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

        private:
            container_type _herds;
    };

    inline Herds::operator std::vector<Herd>() const { return _herds; }
    inline Herds::iterator Herds::begin() { return _herds.begin(); }
    inline Herds::const_iterator Herds::begin() const { return _herds.begin(); }
    inline Herds::iterator Herds::end() { return _herds.end(); }
    inline Herds::const_iterator Herds::end() const { return _herds.end(); }
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

} // namespace portage

#endif /* _HAVE_HERD_H */

/* vim: set tw=80 sw=4 et : */
