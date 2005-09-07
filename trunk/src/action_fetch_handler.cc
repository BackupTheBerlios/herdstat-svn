/*
 * herdstat -- src/action_fetch_handler.cc
 * $Id: action_fetch_handler.cc 508 2005-09-03 11:30:08Z ka0ttic $
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

#include "action_fetch_handler.hh"

int
action_fetch_handler_T::operator() (opts_type &null)
{
    try
    {
        optset("verbose", bool, true);

        herds_xml.fetch(optget("herds.xml", std::string));
        herds_xml.parse(optget("herds.xml", std::string));

        if (use_devaway)
            devaway.fetch(optget("devaway.location", std::string));
    }
    catch (const FetchException)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
