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
#include "io/action/help.hh"
#include "io/batch.hh"

using namespace herdstat;

void
BatchIOHandler::insert_extra_actions(HandlerMap<ActionHandler>& hmap) const
{
    hmap.insert(std::make_pair("help", new HelpActionHandler()));
}

bool
BatchIOHandler::operator()(Query * const query)
{
    Options& options(GlobalOptions());

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

        std::vector<std::string> parts(util::split(in));
        if (parts.empty())
            throw Exception("Failed to parse input!");

        /* copy the global handler map and insert our additional actions */
        static HandlerMap<ActionHandler>
            handlers(GlobalHandlerMap<ActionHandler>());
        insert_extra_actions(handlers);

        ActionHandler *h = handlers[parts[0]];
        if (not h)
            throw ActionUnimplemented(parts[0]);

        query->set_action(parts[0]);
        init_xml_if_necessary(query->action());

        /* transform arguments into the query object */
        if (parts.size() > 1)
            std::transform(parts.begin() + 1, parts.end(),
                std::back_inserter(*query), util::EmptyFirst());
        else if (not h->allow_empty_query())
        {
            query->clear();
            query->push_back(std::make_pair("", h->id()));
            h = handlers["help"];
        }

        QueryResults results;
        (*h)(*query, &results);
    
        QueryResults::iterator i;
        for (i = results.begin() ; i != results.end() ; ++i)
            options.outstream() << i->second << std::endl;
    }
    catch (const ActionUnimplemented& e)
    {
        options.outstream() << "Unknown action '"
            << e.what() << "'.  Try 'help'." << std::endl;
    }

    return true;
}

/* vim: set tw=80 sw=4 fdm=marker et : */
