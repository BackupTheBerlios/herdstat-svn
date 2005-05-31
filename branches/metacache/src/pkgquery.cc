/*
 * herdstat -- src/pkgquery.cc
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

#include "formatter.hh"
#include "pkgquery.hh"

void
pkgQuery_T::dump(std::ostream &stream) const
{
    formatter_T out;
    out.set_maxlabel(16);
    out.set_maxdata(optget("maxcol", std::size_t) - out.maxlabel());
    out.set_attrs();
    
    out("Query string", this->query);
    out("Query with", this->with);
    out("Query type", (this->type == QUERYTYPE_DEV ? "dev":"herd"));
    out("Query date", util::format_date(this->date, "%s") + " ("
        + util::format_date(this->date) + ")");

    for (const_iterator p = this->begin() ; p != this->end() ; ++p)
        out("", p->first);

    out.flush(stream);
}

bool
pkgQuery_T::operator== (const pkgQuery_T &that) const
{
    return  /* normal lookup */
            ((this->query == that.query) and
             (this->with  == that.with) and
             (this->type  == that.type))

            or
            
            /* reverse lookup */
            ((this->query == that.with) and
             (this->with  == that.query) and
             (this->type  != that.type));
}

/* vim: set tw=80 sw=4 et : */
