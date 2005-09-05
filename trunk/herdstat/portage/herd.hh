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

#include <string>
#include <map>
#include <herdstat/portage/developer.hh>

namespace portage {

    /**
     * Represents a herd.
     * Techically a herd is a group of packages, however here it makes more
     * sense to represent them as a group of developers.
     */

    class herd
    {
        public:
            typedef std::map<std::string, developer * > container_type;
            typedef container_type::iterator iterator;
            typedef container_type::const_iterator const_iterator;
            typedef container_type::value_type value_type;
            typedef container_type::key_type key_type;
            typedef container_type::mapped_type mapped_type;
            typedef container_type::size_type size_type;

            herd() { }
            herd(const std::string &name,
                 const std::string &email = "",
                 const std::string &desc  = "");

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
            iterator find(const key_type &k);
            const_iterator find(const key_type &k) const;
            size_type size() const;
            bool empty() const;
            void clear();

        private:
            container_type _herd;
            std::string _name;
            std::string _email;
            std::string _desc;
    };

    inline const std::string& herd::name() const { return _name; }
    inline const std::string& herd::email() const { return _email; }
    inline const std::string& herd::desc() const { return _desc; }
    inline void herd::set_name(const std::string &name) { _name.assign(name); }
    inline void herd::set_email(const std::string &email) { _email.assign(email); }
    inline void herd::set_desc(const std::string &desc) { _desc.assign(desc); }
    inline herd::iterator herd::begin() { return _herd.begin(); }
    inline herd::const_iterator herd::begin() const { return _herd.begin(); }
    inline herd::iterator herd::end() { return _herd.end(); }
    inline herd::const_iterator herd::end() const { return _herd.end(); }
    inline herd::iterator herd::find(const key_type &k) { return _herd.find(k); }
    inline herd::const_iterator herd::find(const key_type &k) const
    { return _herd.find(k); }
    inline herd::size_type herd::size() const { return _herd.size(); }
    inline bool herd::empty() const { return _herd.empty(); }
    inline void herd::clear() { return _herd.clear(); }

    /// herd container.
    typedef std::map<std::string, herd * > herds;

} // namespace portage

#endif /* _HAVE_HERD_HH */

/* vim: set tw=80 sw=4 et : */
