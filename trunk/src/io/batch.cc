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
#include <iterator>
#include <algorithm>

#include <herdstat/util/string.hh>
#include <herdstat/util/functional.hh>

#include "exceptions.hh"
#include "handler_map.hh"
#include "action/handler.hh"
#include "io/action/help.hh"
#include "io/batch.hh"

using namespace herdstat;

BatchIOHandler::BatchIOHandler()
{
    insert_local_handler<HelpIOActionHandler>("help");
}

BatchIOHandler::~BatchIOHandler()
{
}

bool
BatchIOHandler::operator()(Query * const query)
{
    Options& options(GlobalOptions());
    QueryResults results;

    options.set_color(false);
    options.set_quiet(true);

    try
    {
        std::string in;
        if (not std::getline(std::cin, in))
            return false;

        if (in.empty())
            return true;
        if (in == "exit" or in == "quit")
            return false;

        std::vector<std::string> parts;
        util::split(in, std::back_inserter(parts));
        if (parts.empty())
            throw Exception("Failed to parse input!");

        ActionHandler *h = local_handler(parts.front());
        if (not h)
        {
            HandlerMap<ActionHandler>&
                global_handlers(GlobalHandlerMap<ActionHandler>());
            if (not (h = global_handlers[parts.front()]))
                throw ActionUnimplemented(parts.front());
        }

        query->set_action(parts.front());
        parts.erase(parts.begin());
        init_xml_if_necessary(query->action());

        /* transform arguments into the query object */
        if (not parts.empty())
        {
            if (parts.front() == "all")
            {
                query->set_all(true);
                parts.erase(parts.begin());
            }

            std::copy(parts.begin(), parts.end(), std::back_inserter(*query));
        }
        else if (not h->allow_empty_query())
        {
            query->clear();
            query->add(h->id());
            h = local_handler("help");
        }

        (*h)(*query, &results);
        std::transform(results.begin(), results.end(),
            std::ostream_iterator<std::string>(options.outstream(), "\n"),
            util::Second<QuerySpec>());
    }
    catch (const ActionUnimplemented& e)
    {
        options.outstream() << "Unknown action '"
            << e.what() << "'.  Try 'help'." << std::endl;
    }
    catch (const ActionException)
    {
        std::transform(results.begin(), results.end(),
            std::ostream_iterator<std::string>(options.outstream(), "\n"),
            util::Second<QuerySpec>());
    }

    return true;
}

/* vim: set tw=80 sw=4 fdm=marker et : */
