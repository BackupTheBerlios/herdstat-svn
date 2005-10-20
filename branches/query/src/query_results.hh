/*
 * herdstat -- src/query_results.hh
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

#ifndef _HAVE_QUERY_RESULTS_HH
#define _HAVE_QUERY_RESULTS_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <map>
#include <vector>
#include <utility>

/**
 * @class QueryResults
 * @brief Represents a vector of pairs. Each element in the vector describes one
 * match with the map being a list of key/value pairs (fex, "name" => "Aaron
 * Walker").
 */

typedef std::pair<std::string, std::vector<std::string> > Result;

class QueryResults
{
    public:
        typedef std::vector<Result> container_type;
        typedef container_type::iterator iterator;
        typedef container_type::const_iterator const_iterator;
        typedef container_type::reference reference;
        typedef container_type::const_reference const_reference;
        typedef container_type::value_type value_type;
        typedef container_type::size_type size_type;

        /**
         * @name container_type subset
         */
        //@{
        iterator begin() { return _results.begin(); }
        const_iterator begin() const { return _results.begin(); }
        iterator end() { return _results.end(); }
        const_iterator end() const { return _results.end(); }
        size_type size() const { return _results.size(); }
        bool empty() const { return _results.empty(); }
        void push_back(const value_type& v) { _results.push_back(v); }
        iterator insert(iterator pos, const value_type& v)
        { return _results.insert(pos, v); }
        //@}
        
    private:
        container_type _results;
};

#endif /* _HAVE_QUERY_RESULTS_HH */

/* vim: set tw=80 sw=4 et : */
