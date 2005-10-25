/*
 * herdstat -- src/io/batch.cc
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

#include <string>
#include <vector>

#include <herdstat/util/string.hh>
#include <herdstat/util/functional.hh>

#include "exceptions.hh"
#include "handler_map.hh"
#include "action/handler.hh"
#include "io/batch.hh"

using namespace herdstat;

bool
BatchIOHandler::operator()(Query * const query)
{
    Options& options(GlobalOptions());

    options.set_color(false);
    options.set_quiet(true);

    std::string in;
    if (not std::getline(std::cin, in))
        return false;

    if (in.empty())
        return this->operator()(query);
    if (in == "exit" or in == "quit")
        return false;

    std::vector<std::string> parts(util::split(in));
    if (parts.empty())
        throw Exception("Failed to parse input!");

    ActionHandler *h = (GlobalHandlerMap<ActionHandler>())[parts[0]];
    if (not h)
        throw ActionUnimplemented(parts[0]);

    /* transform arguments into the query object */
    if (parts.size() > 1)
        std::transform(parts.begin() + 1, parts.end(),
            std::back_inserter(*query), util::EmptyFirst());

    QueryResults results;
    (*h)(*query, &results);
    
    QueryResults::iterator i;
    for (i = results.begin() ; i != results.end() ; ++i)
        options.outstream() << util::join(i->second) << std::endl;

    return true;
}

/* vim: set tw=80 sw=4 et : */
