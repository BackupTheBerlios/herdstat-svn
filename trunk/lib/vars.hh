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
#include <map>

namespace util
{
    class vars_T
    {
        protected:
            std::string _file;
            std::map<std::string, std::string> _keys;

        public:
            typedef std::map<std::string, std::string>::size_type size_type;
            typedef std::map<std::string, std::string>::iterator iterator;

            vars_T() { }
            vars_T(const char *path) : _file(path) { this->read(); }
            vars_T(const std::string &path) : _file(path) { this->read(); }
            vars_T(std::ifstream &stream) { this->read(stream); }
            
            void read();
            void read(const char *);
            void read(const std::string &);
            void read(std::ifstream &);

            const std::string &filename() const { return _file; }
            
            /* provide a small subset of map methods */
            size_type size() const { return _keys.size(); }
            size_type count(const std::string &s) { return _keys.count(s); }
            iterator begin() { return _keys.begin(); }
            iterator end() { return _keys.end(); }
            iterator find(const std::string &s) { return _keys.find(s); }
            void clear() { _keys.clear(); }
            bool empty() const { return (this->size() == 0); }
            std::string &operator[] (const std::string &s) { return _keys[s]; }
    };
}

#endif

/* vim: set tw=80 sw=4 et : */
