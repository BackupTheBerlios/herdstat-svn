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

#include <locale>
#include <utility>
#include <sys/types.h>

#include "options.hh"

options_T::option_map_T options_T::optmap;

#define insopt(k,v) insert(std::make_pair(k, new option_type_T(v)))

/*
 * Set default options here.
 * This is the only place where new options need to be added.
 */

void
options_T::option_map_T::set_defaults()
{
    insopt("verbose", false);
    insopt("quiet", false);
    insopt("debug", false);
    insopt("timer", false);
    insopt("all", false);
    insopt("fetch", false);
    insopt("dev", false);
    insopt("count", false);
    insopt("color", true);
    insopt("overlay", true);
    insopt("eregex", false);
    insopt("regex", false);
    insopt("qa", false);
    insopt("meta", false);
    insopt("pkgcache", true);

    insopt("maxcol", static_cast<std::size_t>(78));

    insopt("herds.xml", util::string(""));
    insopt("with-herd", util::string(""));
    insopt("with-maintainer", util::string(""));
    insopt("outfile", util::string("stdout"));
    insopt("outstream", &std::cout);

    insopt("action",
        static_cast<options_action_T>(action_unspecified));

    insopt("locale", util::string(std::locale::classic().name()));

    insopt("portage.config", portage::config_T());
}

/* vim: set tw=80 sw=4 et : */
