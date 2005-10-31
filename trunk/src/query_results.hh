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

#include <string>
#include <vector>
#include <utility>

#include <herdstat/util/string.hh>
#include <herdstat/util/container_base.hh>

typedef std::pair<std::string, std::string> Result;

class QueryResults : public herdstat::util::VectorBase<Result>
{
    public:
        void add(const std::string& field, const std::string& val)
        { this->push_back(std::make_pair(field, val)); }
        void add(const std::string& field, const std::vector<std::string>& val)
        { this->push_back(std::make_pair(field, herdstat::util::join(val))); }
};

#endif /* _HAVE_QUERY_RESULTS_HH */

/* vim: set tw=80 sw=4 fdm=marker et : */
