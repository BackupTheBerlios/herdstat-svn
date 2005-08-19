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
    /**
     * Represents a file with variables in the form of VARIABLE=VALUE,
     * stored in key,value pairs.
     */

    class vars : public base_file
    {
        public:
            typedef std::map<std::string, std::string> map_type;
            typedef map_type::iterator iterator;
            typedef map_type::const_iterator const_iterator;
            typedef map_type::mapped_type mapped_type;
            typedef map_type::key_type key_type;
            typedef map_type::value_type value_type;
            typedef map_type::size_type size_type;

            /// Default constructor.
            vars();

            /** Constructor.
             * @param path Path.
             */
            vars(const std::string &path);

            virtual ~vars() { }

            /** Overloaded operator[] since std::map doesn't provide
             * a const version.
             * @param k Key to look up.
             * @returns A string object (Value mapped to Key).
             */
            std::string operator[] (const std::string &k) const;

            /** Dump keys/values to specified stream.
             * @param s Output stream.
             */
            virtual void dump(std::ostream &s) const;

            /// Read file.
            virtual void read();

            /** Read specified file.
             * @param p Path.
             */
            virtual void read(const std::string &p);

        private:
            /** Perform elementary variable substituion.
             * @param v Variable.
             */
            void subst(std::string &v);

            /// subst() recursion depth (safeguard).
            unsigned short _depth;

            /// are we an ebuild?
            bool _ebuild;
            
            map_type _v;
    };

    inline std::string
    vars::operator[] (const std::string &k) const
    {
        const_iterator i = this->_v.find(k);
        return (i == this->_v.end() ? "" : i->second);
    }
}

#endif

/* vim: set tw=80 sw=4 et : */
