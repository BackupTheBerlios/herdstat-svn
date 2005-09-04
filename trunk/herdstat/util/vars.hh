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
#include <utility>
#include <herdstat/util/file.hh>

namespace util
{
    /**
     * Represents a file with variables in the form of VARIABLE=VALUE,
     * stored in key,value pairs.
     */

    class vars_T : public base_file_T
    {
        public:
            typedef std::map<std::string, std::string> container_type;
            typedef container_type::iterator iterator;
            typedef container_type::const_iterator const_iterator;
            typedef container_type::mapped_type mapped_type;
            typedef container_type::key_type key_type;
            typedef container_type::value_type value_type;
            typedef container_type::size_type size_type;

            /// Default constructor.
            vars_T();

            /** Constructor.
             * @param path Path.
             */
            vars_T(const std::string &path);

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

            /// Set default variables to be present before substitution.
            virtual void set_defaults() { }

            iterator begin() { return _vars.begin(); }
            const_iterator begin() const { return _vars.begin(); }
            iterator end() { return _vars.end(); }
            const_iterator end() const { return _vars.end(); }
            size_type size() const { return _vars.size(); }
            bool empty() const { return _vars.empty(); }
            iterator find(const key_type &k) { return _vars.find(k); }
            const_iterator find(const key_type &k) const { return _vars.find(k); }
            mapped_type& operator[] (const key_type &k) { return _vars[k]; }
            std::pair<iterator, bool> insert(const value_type &v)
            { return _vars.insert(v); }
            iterator insert(iterator hpos, const value_type &v)
            { return _vars.insert(hpos, v); }
            template <class In> void insert(In begin, In end)
            { _vars.insert(begin, end); }
            void erase(iterator pos) { _vars.erase(pos); }
            size_type erase(const key_type &k) { return _vars.erase(k); }
            void erase(iterator begin, iterator end)
            { return _vars.erase(begin, end); }
            void clear() { _vars.clear(); }

        private:
            /** Perform elementary variable substitution.
             * @param v Variable.
             */
            void subst(std::string &v);

            /// subst() recursion depth (safeguard).
            unsigned short _depth;
            /// variable container
            container_type _vars;
    };
}

#endif

/* vim: set tw=80 sw=4 et : */
