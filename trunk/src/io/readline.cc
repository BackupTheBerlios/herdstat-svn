/*
 * herdstat -- src/io/readline.cc
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

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <herdstat/util/string.hh>
#include <herdstat/util/functional.hh>

#include "exceptions.hh"
#include "handler_map.hh"
#include "xmlinit.hh"
#include "action/handler.hh"
#include "io/action/set.hh"
#include "io/action/print.hh"
#include "io/action/help.hh"
#include "io/readline.hh"

/// max number of history entries to keep
#define HERDSTAT_HISTORY_MAX 100

using namespace herdstat;
using namespace herdstat::readline;

extern char **herdstat_completion(const char *, int, int);

ReadLineIOHandler::ReadLineIOHandler()
    : _readline(PACKAGE), _history(), _read_hist(false)
{
    const char * const result = std::getenv("HOME");
    if (result)
        _history.set_path(std::string(result) + "/.herdstat_history");

    _readline.set_attempted_comp_hook(herdstat_completion);

    insert_local_handler<HelpIOActionHandler>("help");
    insert_local_handler<SetIOActionHandler>("set");
    insert_local_handler<PrintIOActionHandler>("print");
}

ReadLineIOHandler::~ReadLineIOHandler()
{
    _history.stifle(HERDSTAT_HISTORY_MAX);

    try
    {
        _history.write();
    }
    catch (const FileException& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

bool
ReadLineIOHandler::operator()(Query * const query)
{
    Options& options(GlobalOptions());
    QueryResults results;

    GlobalXMLInit();

    if (not _read_hist)
    {
        _history.read();
        _read_hist = true;
    }

    try
    {
        /* if action is already set, adjust prompt to reflect it */
        if (options.action() == "unspecified")
        {
            options.set_prompt(PACKAGE">");
            query->set_action("unspecified");
        }
        else
        {
            options.set_prompt(std::string(PACKAGE)+":"+options.action()+">");
            query->set_action(options.action());
        }

        _readline.set_prompt(options.prompt());
        std::string in(_readline());

        /* empty, so just call ourselves again and display another prompt */
        if (in.empty())
            return true;

        /* strip trailing whitespace */
        std::string::size_type pos = in.find_last_not_of(" \t");
        if (pos != std::string::npos)
            in.erase(++pos);

        if (in == "quit" or in == "exit")
            return false;

        _history.add(in);

        std::vector<std::string> parts;
        util::split(in, std::back_inserter(parts));
        if (parts.empty())
            /* should never happen since in isn't empty */
	    throw Exception("Failed to parse input!");

        if (query->action() == "unspecified")
        {
            query->set_action(parts.front());
            parts.erase(parts.begin());
        }

        ActionHandler *h = local_handler(query->action());
        if (not h)
        {
            HandlerMap<ActionHandler>&
                global_handlers(GlobalHandlerMap<ActionHandler>());
            HandlerMap<ActionHandler>::iterator i =
                global_handlers.find(query->action());
            if (i == global_handlers.end() or not i->second)
                throw ActionUnimplemented(query->action());
            else
                h = i->second;
        }

        /* transform arguments into the query object */
        if (not parts.empty())
        {
            const std::string& front(parts.front());

            if (front == "all")
            {
                query->set_all(true);
                parts.erase(parts.begin());
            }
            /* action is bound, but we still want to provide access to our
             * handler-specific actions */
            else if (is_local_handler(front))
            {
                h = local_handler(front);
                parts.erase(parts.begin());
            }
            
            std::copy(parts.begin(), parts.end(), std::back_inserter(*query));
        }
        /* no arguments - if the action handler doesn't allow
         * empty query objects then set the action to the 'help'
         * handler and display the help for that action */
        else if (not h->allow_empty_query())
        {
            query->clear();
            query->add(h->id());
            h = local_handler("help");
        }
        
        (*h)(*query, &results);
        display(results);
    }
    catch (const ReadLineEOF&)
    {
        options.outstream() << std::endl;
        return false;
    }
    catch (const ActionUnimplemented& e)
    {
        options.outstream() << "Unknown action '"
            << e.what() << "'.  Try 'help'." << std::endl;
    }
    catch (const ActionException)
    {
        display(results);
    }

    return true;
}

/* vim: set tw=80 sw=4 fdm=marker et : */
