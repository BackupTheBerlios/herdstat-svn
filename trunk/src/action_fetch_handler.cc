/*
 * herdstat -- src/action_fetch_handler.cc
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

#include <iostream>

#include "common.hh"
#include "action_fetch_handler.hh"

using namespace herdstat;

action_fetch_handler_T::~action_fetch_handler_T()
{
}

int
action_fetch_handler_T::operator() (opts_type &null)
{
    if (not options::fields().empty())
    {
        std::cerr << "--field doesn't really make much sense with --fetch."
            << std::endl;
        return EXIT_FAILURE;
    }

    try
    {
        if (not options::quiet())
            options::set_verbose(true);

        fetch_herdsxml();
        /* parse herds.xml so any xml files listed in <maintainingproject>
         * tags will be fetched. */
        herdsxml.parse(options::herdsxml());

        if (options::devaway())
            fetch_devawayxml();
    }
    catch (const FetchException)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
