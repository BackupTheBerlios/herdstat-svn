/*
 * herdstat -- src/io/action/help.cc
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

#include "handler_map.hh"
#include "io/action/help.hh"

using namespace herdstat;

bool
HelpIOActionHandler::allow_empty_query() const
{
    return true;
}

const char * const
HelpIOActionHandler::id() const
{
    return "help";
}

const char * const
HelpIOActionHandler::desc() const
{
    return "Display help.";
}

void
HelpIOActionHandler::generate_completions(std::vector<std::string> *v) const
{
}

void
HelpIOActionHandler::do_results(Query& query,
                                QueryResults * const results)
{
    HandlerMap<ActionHandler>& handlers(GlobalHandlerMap<ActionHandler>());

    /* show general help */
    if (query.empty())
    {
        std::string actions;

        HandlerMap<ActionHandler>::iterator i;
        for (i = handlers.begin() ; i != handlers.end() ; ++i)
            actions.append(i->first + " ");

        results->add("Valid actions: " + actions);
        results->add("Use 'help <action>' to get usage information for the specified action.");
        results->add_linebreak();
    }
    /* show action handler help */
    else
    {
        ActionHandler *h = NULL;
        HandlerMap<ActionHandler>::iterator i = handlers.find(query.front().second);
        if (i != handlers.end())
            h = i->second;

        if (h)
        {
            results->add(util::sprintf("Help for '%s' action handler.", h->id()));
            results->add_linebreak();
            results->add(util::sprintf("Description: %s", h->desc()));
            results->add(util::sprintf("Usage: %s", h->usage()));
        }
        else
        {
            results->add(util::sprintf(
                "Unknown action '%s'.  Run 'help' with no arguments to get a list of valid actions.",
                query.front().second.c_str()));
        }
    }
}

/* vim: set tw=80 sw=4 fdm=marker et : */
