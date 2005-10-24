/*
 * herdstat -- io/handler.cc
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
#include "io/handler.hh"

using namespace herdstat;

PrettyIOHandler::PrettyIOHandler()
    : out(GlobalFormatter()), attrs(out.attrs())
{
    /* set common format attributes */
    attrs.set_maxlen(options::maxcol());
    attrs.set_quiet(options::quiet());
    attrs.set_colors(options::color());

    /* add highlights */
    attrs.add_highlight(util::current_user());
    attrs.add_highlight(util::get_user_from_email(util::current_user()));
    /* user-defined highlights */
    attrs.add_highlights(util::split(options::highlights()));
}

bool
PrettyIOHandler::output(const QueryResults& results)
{
    QueryResults::const_iterator i;
    for (i = results.begin() ; i != results.end() ; ++i)
        out(i->first, i->second);

    out.flush(*options::outstream());

    return true;
}

/* vim: set tw=80 sw=4 et : */
