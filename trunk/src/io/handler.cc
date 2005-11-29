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

#include <algorithm>
#include <herdstat/util/string.hh>

#include "common.hh"
#include "xmlinit.hh"
#include "io/action/help.hh"
#include "io/action/set.hh"
#include "io/action/print.hh"
#include "io/handler.hh"

using namespace herdstat;

IOHandler::IOHandler()
    : _local()
{
}

IOHandler::~IOHandler()
{
    std::for_each(_local.begin(), _local.end(),
        util::compose_f_gx(
            util::DeleteAndNullify<ActionHandler>(),
            util::Second<HandlerMap<ActionHandler>::value_type>()));
}

void
IOHandler::init_xml_if_necessary(const std::string& action)
{
    static char *actions[] = { "away", "dev", "herd", "pkg", "stats" };
    if (std::binary_search(actions, actions+NELEMS(actions), action))
        GlobalXMLInit();
}

/* vim: set tw=80 sw=4 fdm=marker et : */
