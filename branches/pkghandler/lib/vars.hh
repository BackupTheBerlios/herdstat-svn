/*
 * herdstat -- lib/vars.hh
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

#ifndef HAVE_VARS_HH
#define HAVE_VARS_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <fstream>
#include <utility>
#include <map>

#include "file.hh"

namespace util
{
    class vars_T : public util::file_T
    {
        public:
            typedef util::file_T::string_type string_type;
            typedef std::map<string_type, string_type> value_type;
            typedef value_type::size_type size_type;
            typedef value_type::iterator iterator;
            typedef value_type::const_iterator const_iterator;

            vars_T() : _depth(0) { }
            vars_T(const util::path_T &path) : util::file_T(path), _depth(0)
            {
                this->open();
                this->read();
            }

            /* map subset */
            size_type count(const string_type &s)
            { return this->_keys.count(s); }
            iterator begin() { return this->_keys.begin(); }
            const_iterator begin() const { return this->_keys.begin(); }
            iterator end() { return this->_keys.end(); }
            const_iterator end() const { return this->_keys.end(); }
            iterator find(const string_type &s) { return this->_keys.find(s); }
            const_iterator find(const string_type &s) const
            { return this->_keys.find(s); }
            void clear() { this->_keys.clear(); }
            bool empty() const { return (this->size() == 0); }

            /* work around the fact that std::map doesn't have a
             * operator[]() const */
            util::string operator[] (const string_type &s) const
            {
                const_iterator i = this->find(s);
                return (i == this->end() ? "" : i->second);
            }

            util::string operator[] (const string_type &s)
            { return this->_keys[s]; }

            virtual void read();
            virtual void read(const util::path_T &);

        private:
            void subst(string_type &);

            unsigned short _depth;  /* subst() recursion depth */
            bool _ebuild;           /* are we an ebuild? */
            value_type _keys;       /* var map */
    };
}

#endif

/* vim: set tw=80 sw=4 et : */
