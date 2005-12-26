/*
 * herdstat -- src/io/readline_completion_hooks.cc
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

#include <cstdlib>
#include <cstring>
#include <herdstat/util/readline.hh>

#include "common.hh"
#include "action/handler.hh"
#include "handler_map.hh"

static std::string cur_action;

// Complete on action names
static char *
action_completion(const char *text, int state)
{
    static const char * const local[] = {"help", "set", "print", "quit", "exit"};
    static std::size_t lit; /* local iterator */
    static HandlerMap<ActionHandler>::iterator hit; /* handlers iterator */
    static const HandlerMap<ActionHandler>::iterator hend =
        GlobalHandlerMap<ActionHandler>().end(); /* handlers end iterator */
    static std::size_t len; /* text length - saved for efficiency */

    if (state == 0)
    {
        hit = GlobalHandlerMap<ActionHandler>().begin();
        lit = 0;
        len = std::strlen(text);
    }

    /* search global handlers */
    for ( ; hit != hend ; ++hit)
    {
        if (hit->first.substr(0, len) == text)
            return strdup(hit++->first.c_str());
    }

    /* search local handlers */
    for ( ; lit < NELEMS(local) ; ++lit)
    {
        if (std::strncmp(local[lit], text, len) == 0)
            return strdup(local[lit++]);
    }

    return NULL;
}

// Complete on action paramaters
static char *
action_param_completion(const char *text, int state)
{
    static std::size_t offset, len;
    static std::vector<std::string> comps;

    assert(not cur_action.empty());

    /* parameters for help are the action handler names */
    if (cur_action == "help")
        return action_completion(text, state);

    if (state == 0)
    {
        offset = 0;
        len = std::strlen(text);

        comps.clear();

        HandlerMap<ActionHandler>& handlers(GlobalHandlerMap<ActionHandler>());
        HandlerMap<ActionHandler>::iterator i = handlers.find(cur_action);
        if ((i == handlers.end()) or (i->second == NULL))
            return NULL;

        i->second->generate_completions(&comps);
    }

    std::vector<std::string>::const_iterator c = comps.begin() + offset;

    for ( ; c != comps.end() ; ++c)
    {
        ++offset;
        if (c->substr(0, len) == text)
            return strdup(c->c_str());
    }

    return NULL;
}

// main completion logic ; decides which completion entry function to call
char **
herdstat_completion(const char *text, int start, int end)
{
    const Options& options(GlobalOptions());
    char **matches = NULL;

    /* Don't have readline use default completion methods (file/directory
     * completion) if this function returns NULL */
    rl_attempted_completion_over = 1;

    /* if at first word and action is not bound, complete on actions */
    if (start == 0 and options.action() == "unspecified")
        matches = rl_completion_matches(text, action_completion);
    /* otherwise determine action and complete on it's parameters */
    else
    {
        /* action is bound */
        if (start == 0)
            cur_action.assign(options.action());
        /* action is not bound, so get it from rl_line_buffer */
        else
        {
            /* get first word in rl_line_buffer */
            const char * const space = std::strchr(rl_line_buffer, ' ');
            if (space)
                cur_action.assign(rl_line_buffer,
                                  rl_line_buffer + (space - rl_line_buffer));
            else
                cur_action.assign(rl_line_buffer);
        }

        matches = rl_completion_matches(text, action_param_completion);
    }

    return matches;
}

/* vim: set tw=80 sw=4 fdm=marker et : */
