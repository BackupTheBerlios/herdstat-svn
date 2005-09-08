/*
 * herdstat -- portage/developer.hh
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

#ifndef _HAVE_DEVELOPER_HH
#define _HAVE_DEVELOPER_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/**
 * @file developer.hh
 * @brief Defines the developer class.
 */

#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <herdstat/util/regex.hh>
#include <herdstat/portage/gentoo_email_address.hh>

namespace portage {

    /**
     * Represents a Gentoo developer.
     */

    class Developer
    {
        public:
            /// Default constructor.
            Developer();

            /** Constructor.
             * @param email email address.
             * @param name  name.
             */
            Developer(const std::string &user,
                      const std::string &email = "",
                      const std::string &name = "");

            /** Implicit conversion to std::string.
             * @returns Developer user name.
             */
            operator std::string() const;

            /** Determine if this developer is equal to that developer.
             * @param user user name.
             * @returns True if user name is equivelent.
             */
            bool operator== (const std::string& user) const;
            bool operator== (const Developer& that) const;
            bool operator!= (const std::string& user) const;
            bool operator!= (const Developer& that) const;

            const std::string& user() const;
            const std::string& email() const;
            const std::string& name() const;
            const std::string& pgpkey() const;
            const std::string& joined() const;
            const std::string& birthday() const;
            const std::string& status() const;
            const std::string& role() const;
            const std::string& location() const;
            const std::string& awaymsg() const;
            const std::vector<std::string>& herds() const;
            bool is_away() const;

            void set_user(const std::string& user);
            void set_email(const std::string& email);
            void set_name(const std::string& name);
            void set_pgpkey(const std::string& pgpkey);
            void set_joined(const std::string& date);
            void set_birthday(const std::string& date);
            void set_status(const std::string& status);
            void set_role(const std::string& role);
            void set_location(const std::string& location);
            void set_awaymsg(const std::string& msg);
            void set_herds(const std::vector<std::string>& herds);
            void append_herd(const std::string& herd);
            void set_away(const bool away);

        private:
            std::string _user;
            GentooEmailAddress _email;
            std::string _name;
            std::string _pgpkey;
            std::string _joined;
            std::string _birth;
            std::string _status;
            std::string _role;
            std::string _location;
            std::string _awaymsg;
            bool _away;
            std::vector<std::string> _herds;
    };

    inline Developer::operator std::string() const { return _user; }
    inline bool Developer::operator== (const std::string& user) const
    { return (_user == user); }
    inline bool Developer::operator== (const Developer& that) const
    { return (_user == that._user); }
    inline bool Developer::operator!= (const std::string& user) const
    { return not (*this == user); }
    inline bool Developer::operator!= (const Developer& that) const
    { return not (*this == that); }

    inline const std::string& Developer::user() const { return _user; }
    inline const std::string& Developer::email() const { return _email.str(); }
    inline const std::string& Developer::name() const { return _name; }
    inline const std::string& Developer::pgpkey() const { return _pgpkey; }
    inline const std::string& Developer::joined() const { return _joined; }
    inline const std::string& Developer::birthday() const { return _birth; }
    inline const std::string& Developer::status() const { return _status; }
    inline const std::string& Developer::role() const { return _role; }
    inline const std::string& Developer::location() const { return _location; }
    inline const std::string& Developer::awaymsg() const { return _awaymsg; }
    inline bool Developer::is_away() const { return _away; }
    inline const std::vector<std::string>& Developer::herds() const
    { return _herds; }

    inline void Developer::set_user(const std::string& user)
    { _user.assign(user); }
    inline void Developer::set_email(const std::string& email)
    { _email.assign(email); }
    inline void Developer::set_name(const std::string& name)
    { _name.assign(name); }
    inline void Developer::set_pgpkey(const std::string& pgpkey)
    { _pgpkey.assign(pgpkey); }
    inline void Developer::set_joined(const std::string& date)
    { _joined.assign(date); }
    inline void Developer::set_birthday(const std::string& date)
    { _birth.assign(date); }
    inline void Developer::set_status(const std::string& status)
    { _status.assign(status); }
    inline void Developer::set_role(const std::string& role)
    { _role.assign(role); }
    inline void Developer::set_location(const std::string& location)
    { _location.assign(location); }
    inline void Developer::set_awaymsg(const std::string& msg)
    { _awaymsg.assign(msg); }
    inline void Developer::set_herds(const std::vector<std::string>& herds)
    { _herds = herds; }
    inline void Developer::append_herd(const std::string& herd)
    { _herds.push_back(herd); }
    inline void Developer::set_away(const bool away) { _away = away; }

    /**
     * Developer container.
     */

    class Developers
    {
        public:
            typedef std::vector<Developer> container_type;
            typedef container_type::iterator iterator;
            typedef container_type::const_iterator const_iterator;
            typedef container_type::value_type value_type;
            typedef container_type::size_type size_type;

            Developers();
            Developers(const std::vector<std::string>& v);
            virtual ~Developers() { }

            /// Implicit conversion to std::vector<std::string>.
            operator std::vector<std::string>() const;

            Developers& operator= (const std::vector<Developer>& v);
            Developers& operator= (const std::vector<std::string>& v);

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
            const value_type& operator[](size_type pos) const;

        private:
            container_type _devs;
    };
    inline Developers& Developers::operator= (const std::vector<Developer>& v)
    { _devs = v; return *this; }
    inline Developers::iterator Developers::begin() { return _devs.begin(); }
    inline Developers::const_iterator Developers::begin() const { return _devs.begin(); }
    inline Developers::iterator Developers::end() { return _devs.end(); }
    inline Developers::const_iterator Developers::end() const { return _devs.end(); }
    inline Developers::size_type Developers::size() const { return _devs.size(); }
    inline bool Developers::empty() const { return _devs.empty(); }
    inline void Developers::clear() { return _devs.clear(); }
    inline void Developers::push_back(const Developer& dev) { _devs.push_back(dev); }
    inline void Developers::push_back(const std::string& email)
    { _devs.push_back(Developer(email)); }

    inline Developers::iterator Developers::find(const std::string& dev)
    { return std::find(_devs.begin(), _devs.end(), dev.substr(0, dev.find('@'))); }
    inline Developers::const_iterator Developers::find(const std::string& dev) const
    { return std::find(_devs.begin(), _devs.end(), dev.substr(0, dev.find('@'))); }
    inline Developers::iterator Developers::find(const Developer& dev)
    { return std::find(_devs.begin(), _devs.end(), dev); }
    inline Developers::const_iterator Developers::find(const Developer& dev) const
    { return std::find(_devs.begin(), _devs.end(), dev); }
    inline Developers::iterator Developers::find(const util::regex_T &regex)
    { return std::find_if(_devs.begin(), _devs.end(),
            std::bind1st(util::regexMatch(), &regex)); }
    inline Developers::const_iterator Developers::find(const util::regex_T &regex) const
    { return std::find_if(_devs.begin(), _devs.end(),
            std::bind1st(util::regexMatch(), &regex)); }
    template <class In> inline void
    Developers::insert(iterator pos, In begin, In end)
    { _devs.insert(pos, begin, end); }
    inline const Developers::value_type& Developers::operator[] (size_type pos) const
    { return _devs[pos]; }


} // namespace portage

#endif /* _HAVE_DEVELOPER_HH */

/* vim: set tw=80 sw=4 et : */
