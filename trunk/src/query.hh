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

#include <herdstat/util/container_base.hh>
#include "options.hh"

/*
 * Represents a list of conditions that whatever is being searched must meet in
 * order to be considered a match.  Holds key/value pairs, the name of the field
 * we're searching (name, for example), to the criteria (Aaron Walker, for
 * example).  The 'first' member may be empty.
 */

class Query
    : public herdstat::util::VectorBase<std::pair<std::string, std::string> >
{
    public:
        typedef herdstat::util::VectorBase<std::pair<std::string, std::string> > \
            base_type;

        Query();

        /* get action associated with this query */
        const std::string& action() const { return _action; }
        void set_action(const std::string& v) { _action.assign(v); }

        /* special condition - match all. */
        bool all() const { return _all; }
        void set_all(bool v) { _all = v; }

//        iterator find(const std::string& k);
//        const_iterator find(const std::string& k) const;

    private:
        std::string _action;
        bool _all;
};

#endif /* _HAVE_QUERY_HH */

/* vim: set tw=80 sw=4 fdm=marker et : */
