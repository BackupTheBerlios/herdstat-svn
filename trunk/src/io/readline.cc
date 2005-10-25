/*
 * herdstat -- src/readline.cc
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

#ifdef HAVE_READLINE_READLINE_H
# include <readline/readline.h>
# include <readline/history.h>
#endif

#include <herdstat/util/string.hh>
#include <herdstat/util/functional.hh>

#include "exceptions.hh"
#include "handler_map.hh"
#include "action/handler.hh"
#include "io/readline.hh"

using namespace herdstat;

class ReadlineEOF : public Exception { };

static const std::string *rl_buffer = NULL;

static int
readline_init()
{
    rl_insert_text(rl_buffer->c_str());
    return 0;
}

/* readline() wrapper */
static void get_input(const std::string&, std::string *,
                      const std::string& = "");

void
get_input(const std::string& prompt, std::string *result,
          const std::string& text)
{
    /* for readline_init startup hook */
    rl_buffer = &text;
    
    char *input = readline(prompt.c_str());
    if (not input)
        throw ReadlineEOF();

    if (*input)
    {
        add_history(input);
        result->assign(input);
    }
    
    std::free(input);
}

ReadLineIOHandler::ReadLineIOHandler()
{
    rl_startup_hook = readline_init;
    /* no completions (for now) */
    rl_bind_key('\t', NULL);
}

bool
ReadLineIOHandler::operator()(Query * const query)
{
    Options& options(GlobalOptions());

    try
    {
        std::string in;

        /* TODO: allow action binding and show current action in the prompt. */
        get_input(PACKAGE"> ", &in);

        /* empty, so just call ourselves again and display another prompt */
        if (in.empty())
            return this->operator()(query);
        if (in == "quit" or in == "exit")
            return true;

        std::vector<std::string> parts(util::split(in));
        if (parts.empty())
	    return false;

        ActionHandler *h = (GlobalHandlerMap<ActionHandler>())[parts[0]];
        if (not h)
            throw ActionUnimplemented(parts[0]);

        /* assign arguments */
        if (parts.size() > 1)
            std::transform(parts.begin() + 1, parts.end(),
                std::back_inserter(*query), util::EmptyFirst());

        QueryResults results;
        (*h)(*query, &results);
        display(results);
    }
    catch (const ReadlineEOF)
    {
        options.outstream() << std::endl;
        return true;
    }
    catch (const ActionUnimplemented& e)
    {
        options.outstream() << "Unknown action '"
            << e.what() << "'.  Try 'help'." << std::endl;
        return this->operator()(query);
    }

    return true;
}

/* vim: set tw=80 sw=4 et : */
