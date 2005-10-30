/*
 * herdstat -- src/getcols.cc
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

#ifdef HAVE_NCURSES

#include <curses.h>
#include <term.h>

static bool term_init = false;
static char term_info[2048];

#endif

#include <herdstat/exceptions.hh>

using namespace herdstat;

/* 
 * Get terminal width.
 */

std::string::size_type
getcols()
{
#ifdef HAVE_NCURSES

    if (not term_init)
    {
        const char *type = std::getenv("TERM");
        /* default to 'vt100' */
        if (not type)
        {
            if (setenv("TERM", "vt100", 0) != 0)
                throw Exception("setenv(\"TERM\", \"vt100\") failed");
        }

        int result = tgetent(term_info, std::getenv("TERM"));
        if (result < 0)
            throw Exception("Failed to access termcap database.");
        else if (result == 0)
            throw Exception("Unknown terminal type '%s'", type);

        term_init = true;
    }

    int cols = tgetnum("co");
    if (cols > 0)
        return cols;

#else /* HAVE_NCURSES */

    std::string output;
    FILE *p = popen("stty size 2>/dev/null", "r");
    if (p)
    {
	char line[10];
	if (std::fgets(line, sizeof(line) - 1, p) != NULL)
	    output = line;
	pclose(p);
    }

    if (not output.empty())
    {
	std::string::size_type pos;
	if ((pos = output.find(" ")) != std::string::npos)
	    return std::atoi(output.substr(pos).c_str());
    }

#endif /* HAVE_NCURSES */

    return 78;
}

/* vim: set tw=80 sw=4 fdm=marker et : */
