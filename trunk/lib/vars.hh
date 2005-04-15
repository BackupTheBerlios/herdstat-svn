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
#include <string>
#include <utility>
#include <map>
#include "file.hh"

namespace util
{
    class vars_T : public util::file_T
    {
        private:
            void subst(std::string &);

            bool _ebuild;
            std::map<std::string, std::string> _keys;

        public:
            typedef std::map<std::string, std::string>::size_type size_type;
            typedef std::map<std::string, std::string>::iterator iterator;

            vars_T() { }
            vars_T(const util::path_T &path) : util::file_T(path)
            {
                this->open();
                this->read();
            }

            /* map subset */
            size_type count(const std::string &s)
            { return this->_keys.count(s); }
            iterator begin() { return this->_keys.begin(); }
            iterator end() { return this->_keys.end(); }
            iterator find(const std::string &s) { return this->_keys.find(s); }
            void clear() { this->_keys.clear(); }
            bool empty() const { return (this->size() == 0); }
            std::string &operator[] (const std::string &s) { return this->_keys[s]; }

            virtual void read();
            virtual void read(const util::path_T &);
    };
}

#endif

/* vim: set tw=80 sw=4 et : */
