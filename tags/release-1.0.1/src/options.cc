/*
 * herdstat -- src/options.cc
 * $Id$
 * Copyright (c) 2005 Aaron Walker <ka0ttic at gentoo.org>
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
#include <locale>
#include <string>
#include <map>
#include <utility>
#include <sys/types.h>
#include "options.hh"

options_T::option_map_T options_T::optmap;

#define insert_opt(k,v) insert(std::make_pair(k, new option_type_T(v)))

/*
 * Set default options here.
 * This is the only place where new options need to be added.
 */

void
options_T::option_map_T::set_defaults()
{
    insert_opt("verbose", false);
    insert_opt("quiet", false);
    insert_opt("debug", false);
    insert_opt("timer", false);
    insert_opt("all", false);
    insert_opt("fetch", false);
    insert_opt("dev", false);
    insert_opt("count", false);
    insert_opt("color", true);
    insert_opt("parse herds.xml", true);
    insert_opt("no-herd", false);

    insert_opt("maxcol", static_cast<std::size_t>(78));

    insert_opt("portdir", std::string("/usr/portage"));
    insert_opt("outfile", std::string("stdout"));
    insert_opt("herds.xml",
        std::string("http://www.gentoo.org/cgi-bin/viewcvs.cgi/misc/herds.xml?rev=HEAD;cvsroot=gentoo;content-type=text/plain"));
    
    insert_opt("outstream", &std::cout);

    insert_opt("action",
        static_cast<options_action_T>(action_unspecified));

    insert_opt("locale", std::locale("").name());
}

/* vim: set tw=80 sw=4 et : */
