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
#include "file.hh"

namespace util
{
    /**
     * Represents a file with variables in the form of VARIABLE=VALUE,
     * stored in key,value pairs.
     */

    class vars_T : public base_file_T,
                   public std::map<std::string, std::string>
    {
        public:
            /// Default constructor.
            vars_T() : _depth(0) { }

            /** Constructor.
             * @param path Path.
             */
            vars_T(const std::string &path) : base_file_T(path), _depth(0)
            { this->read(); }

            virtual ~vars_T() { }

            /** Overloaded operator[] since std::map doesn't provide
             * a const version.
             * @param k Key to look up.
             * @returns A std::string object (Value mapped to Key).
             */
            std::string operator[] (const std::string &k) const
            {
                const_iterator i = this->find(k);
                return (i == this->end() ? "" : i->second);
            }

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
    };
}

#endif

/* vim: set tw=80 sw=4 et : */
