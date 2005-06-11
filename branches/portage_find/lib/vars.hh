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

#include <map>
#include "string.hh"
#include "file.hh"

namespace util
{
    class vars_T : public base_file_T,
                   public std::map<string, string>
    {
        public:
            vars_T() : _depth(0) { }
            vars_T(const path_T &path) : base_file_T(path), _depth(0)
            { this->read(); }

            virtual ~vars_T() { }

            /* work around the fact that std::map doesn't have a
             * operator[]() const */
            string operator[] (const string &s) const
            {
                const_iterator i = this->find(s);
                return (i == this->end() ? "" : i->second);
            }

            virtual void dump(std::ostream &) const;
            virtual void read();
            virtual void read(const path_T &);

        private:
            void subst(string &);

            unsigned short _depth;  /* subst() recursion depth */
            bool _ebuild;           /* are we an ebuild? */
    };
}

#endif

/* vim: set tw=80 sw=4 et : */
