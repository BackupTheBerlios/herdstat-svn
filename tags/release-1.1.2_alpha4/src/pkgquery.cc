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

#include <herdstat/util/string.hh>
#include <herdstat/portage/config.hh>
#include "formatter.hh"
#include "pkgquery.hh"
#include "options.hh"

pkgQuery_T::pkgQuery_T(const std::string &n, const std::string &w, bool dev)
    : _pkgs(), info(n), query(n), with(w), portdir(options::portdir()),
      overlays(options::overlays()), type(dev? QUERYTYPE_DEV : QUERYTYPE_HERD)
{
}

void
pkgQuery_T::dump(std::ostream &stream) const
{
    formatter_T out;
    out.set_maxlabel(16);
    out.set_maxdata(options::maxcol() - out.maxlabel());
    out.set_attrs();
    
    out("Query string", this->query);
    out("Query with", this->with);
    out("Query type", (this->type == QUERYTYPE_DEV ? "dev":"herd"));
    out("Query Portdir", this->portdir);
    out("Query date", util::sprintf("%lu", static_cast<unsigned long>(this->date))
        + " (" + util::format_date(this->date) + ")");

    if (not this->empty())
        out(util::sprintf("Results(%d)", this->size()),
            this->begin()->first);

    if (this->size() > 1)
    {
        const_iterator p = this->begin();
        ++p;
        for ( ; p != this->end() ; ++p)
            out("", p->first);
    }

    out.flush(stream);
}

bool
pkgQuery_T::operator== (const pkgQuery_T &that) const
{
    debug_msg(" pkgQuery_T::operator==");
    debug_msg("   this->query(%s) == that.query(%s) ? %d",
        this->query.c_str(), that.query.c_str(), (this->query == that.query));
    debug_msg("   this->with(%s) == that.with(%s) ? %d",
        this->with.c_str(), that.with.c_str(), (this->with == that.with));
    debug_msg("   this->type(%d) == that.type(%d) ? %d",
        this->type, that.type, (this->type == that.type));
    debug_msg("   this->portdir(%s) == that.portdir(%s) ? %d",
        this->portdir.c_str(), that.portdir.c_str(), (this->portdir == that.portdir));
    debug_msg("   this->overlays == that.overlays ? %d",
            (this->overlays == that.overlays));

    return  /* normal lookup */
            ((this->query == that.query) and
             (this->with  == that.with) and
             (this->type  == that.type) and
             (this->portdir == that.portdir) and
             (this->overlays == that.overlays))

            or
            
            /* reverse lookup */
            ((this->query == that.with) and
             (this->with  == that.query) and
             (this->type  != that.type));
}

/*
 * Return a vector of the packages we contain.
 */

std::vector<std::string>
pkgQuery_T::pkgs() const
{
    std::vector<std::string> v;
    for (const_iterator i = this->begin() ; i != this->end() ; ++i)
        v.push_back(i->first);
    return v;
}

/* vim: set tw=80 sw=4 et : */
