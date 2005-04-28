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
        private:
            void subst(util::string &);

            unsigned short _depth;  /* subst() recursion depth */
            bool _ebuild;           /* are we an ebuild? */
            std::map<util::string, util::string> _keys; /* var map */

        public:
            typedef std::map<util::string, util::string>::size_type size_type;
            typedef std::map<util::string, util::string>::iterator iterator;
            typedef std::map<util::string, util::string>::const_iterator
                                                        const_iterator;

            vars_T() : _depth(0) { }
            vars_T(const util::path_T &path) : util::file_T(path), _depth(0)
            {
                this->open();
                this->read();
            }

            /* map subset */
            size_type count(const util::string &s)
            { return this->_keys.count(s); }
            iterator begin() { return this->_keys.begin(); }
            const_iterator begin() const { return this->_keys.begin(); }
            iterator end() { return this->_keys.end(); }
            const_iterator end() const { return this->_keys.end(); }
            iterator find(const util::string &s) { return this->_keys.find(s); }
            const_iterator find(const util::string &s) const
            { return this->_keys.find(s); }
            void clear() { this->_keys.clear(); }
            bool empty() const { return (this->size() == 0); }
            util::string &operator[] (const util::string &s)
            { return this->_keys[s]; }

            virtual void read();
            virtual void read(const util::path_T &);
    };
}

#endif

/* vim: set tw=80 sw=4 et : */
