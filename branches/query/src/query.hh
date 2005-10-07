/*
 * herdstat -- src/query.hh
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

#ifndef _HAVE_QUERY_HH
#define _HAVE_QUERY_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <vector>
#include <utility>
#include <string>

enum ActionMethod;

/**
 * @class Query
 * @brief Represents a list of conditions that whatever is being searched must
 * meet in order to be considered a match.  Holds key/value pairs, the name of
 * the field we're searching (name, fex), to the criteria (Aaron Walker, fex).
 */

class Query
{
    public:
        typedef std::vector<std::pair<std::string, std::string> > container_type;
        typedef container_type::iterator iterator;
        typedef container_type::const_iterator const_iterator;
        typedef container_type::reference reference;
        typedef container_type::const_reference const_reference;
        typedef container_type::value_type value_type;
        typedef container_type::size_type size_type;

        /// Default constructor.
        Query();

        /// Get action associated with this query.
        inline ActionMethod action() const;

        //@{
        /// Special condition - match all.
        inline bool all() const;
        inline void set_all(bool v);
        //@}

        /**
         * @name container_type subset
         */
        //@{
        iterator find(const std::string& k);
        const_iterator find(const std::string& k) const;

        inline iterator begin();
        inline const_iterator begin() const;
        inline iterator end();
        inline const_iterator end() const;

        inline size_type size() const;
        inline bool empty() const;
        //@}

    private:
        container_type _qm;
        ActionMethod _action;
        bool _all;
};

inline ActionMethod Query::action() const { return _action; }
inline bool Query::all() const { return _all; }
inline void Query::set_all(bool v) { _all = v; }
inline Query::iterator Query::begin() { return _qm.begin(); }
inline Query::const_iterator Query::begin() const { return _qm.begin(); }
inline Query::iterator Query::end() { return _qm.end(); }
inline Query::const_iterator Query::end() const { return _qm.end(); }
inline Query::size_type Query::size() const { return _qm.size(); }
inline bool Query::empty() const { return _qm.empty(); }

#endif /* _HAVE_QUERY_HH */

/* vim: set tw=80 sw=4 et : */
