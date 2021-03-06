/*
 * herdstat -- src/io/action/handler.cc
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

#include "exceptions.hh"
#include "io/action/handler.hh"
#include <herdstat/defs.hh>

using namespace gui;

bool
IOActionHandler::allow_empty_query() const
{
    return true;
}

void
IOActionHandler::do_all(Query& query LIBHERDSTAT_UNUSED,
                        QueryResults * const results)
{
    results->add("This action does not support the 'all' target.");
    throw ActionException();
}

void
IOActionHandler::do_regex(Query& query LIBHERDSTAT_UNUSED,
                          QueryResults * const results LIBHERDSTAT_UNUSED)
{
}

Tab *
IOActionHandler::createTab(WidgetFactory* widgetFactory LIBHERDSTAT_UNUSED)
{
    /* by default, these action handlers dont have tabs as the majority
     * of these actions are non-gui actions. */
    return NULL;
}

/* vim: set tw=80 sw=4 fdm=marker et : */
