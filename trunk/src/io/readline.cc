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
ReadLineIOHandler::input(Query * const query)
{
    std::string in;
    
    /* try to read input */
    try
    {
        get_input(PACKAGE"> ", &in);
    }
    catch (const ReadlineEOF)
    {
        *options::outstream() << std::endl;
        return false;
    }

    /* empty, so just call ourselves again and display another prompt */
    if (in.empty())
        return input(query);
    if (in == "quit" or in == "exit")
        return false;

    std::vector<std::string> parts(util::split(in));
    if (parts.empty())
	return false;

    /* set action accordingly */
    if (parts[0] == "away")
	options::set_action(action_away);
    else if (parts[0] == "dev")
	options::set_action(action_dev);
    else if (parts[0] == "fetch")
	options::set_action(action_fetch);
    else if (parts[0] == "find")
	options::set_action(action_find);
    else if (parts[0] == "herd")
	options::set_action(action_herd);
    else if (parts[0] == "keywords")
	options::set_action(action_kw);
    else if (parts[0] == "meta")
	options::set_action(action_meta);
    else if (parts[0] == "pkg")
	options::set_action(action_pkg);
    else if (parts[0] == "stats")
	options::set_action(action_stats);
    else if (parts[0] == "versions")
	options::set_action(action_versions);
    else if (parts[0] == "which")
	options::set_action(action_which);
    else
    {
	std::cerr << "Unknown action.  Try 'help'." << std::endl;
	return input(query);
    }

    /* assign arguments */
    if (parts.size() > 1)
        std::transform(parts.begin() + 1, parts.end(),
            std::back_inserter(*query), util::EmptyFirst());

    return true;
}

/* vim: set tw=80 sw=4 et : */
