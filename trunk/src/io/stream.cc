/*
 * herdstat -- io/stream.cc
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

#include <herdstat/util/misc.hh>
#include "exceptions.hh"
#include "handler_map.hh"
#include "action/handler.hh"
#include "io/stream.hh"

using namespace herdstat;

bool
StreamIOHandler::operator()(Query * const query)
{
    QueryResults results;

    try
    {
        ActionHandler *h = (GlobalHandlerMap<ActionHandler>())[query->action()];
        if (not h)
            throw ActionUnimplemented(query->action());

        init_xml_if_necessary(query->action());

        if (query->empty() and h->allow_pwd_query())
            h->handle_pwd_query(query, &results);

        (*h)(*query, &results);
        display(results);
    }
    catch (const ActionException)
    {
        display(results);
        throw;
    }

    return false;
}

/* vim: set tw=80 sw=4 fdm=marker et : */
