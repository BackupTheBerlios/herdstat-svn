/*
 * herdstat -- src/query.cc
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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <algorithm>
#include "query.hh"

Query::iterator
Query::find(const std::string& k)
{
    for (iterator i = _qm.begin() ; i != _qm.end() ; ++i)
        if (i->first == k) return i;
    return _qm.end();
}

Query::const_iterator
Query::find(const std::string& k) const
{
    for (const_iterator i = _qm.begin() ; i != _qm.end() ; ++i)
        if (i->first == k) return i;
    return _qm.end();
}

/* vim: set tw=80 sw=4 et : */
