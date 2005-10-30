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

#include <memory>
#include <herdstat/util/string.hh>

#include "exceptions.hh"
#include "handler_map.hh"
#include "action/handler.hh"
#include "io/handler.hh"

using namespace herdstat;

PrettyIOHandler::PrettyIOHandler()
    : out(GlobalFormatter()), attrs(out.attrs())
{
    const Options& opts(GlobalOptions());

    /* set common format attributes */
    attrs.set_maxlen(opts.maxcol());
    attrs.set_quiet(opts.quiet());
    attrs.set_colors(opts.color());

    /* add highlights */
    const std::string user(util::current_user());
    attrs.add_highlight(user);
    attrs.add_highlight(util::get_user_from_email(user));
    /* user-defined highlights */
    attrs.add_highlights(util::split(opts.highlights()));
}

void
PrettyIOHandler::display(const QueryResults& results)
{
    QueryResults::const_iterator i;
    for (i = results.begin() ; i != results.end() ; ++i)
        out(i->first, i->second);

    out.flush(GlobalOptions().outstream());
}

/* vim: set tw=80 sw=4 fdm=marker et : */
