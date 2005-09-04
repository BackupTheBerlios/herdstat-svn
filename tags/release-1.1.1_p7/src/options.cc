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
    insopt("dev", false);
    insopt("count", false);
    insopt("color", true);
    insopt("overlay", true);
    insopt("eregex", false);
    insopt("regex", false);
    insopt("qa", false);
    insopt("meta", false);
    insopt("metacache", true);
    insopt("querycache", true);
    insopt("devaway", true);

    insopt("label.color", std::string("green"));
    insopt("highlight.color", std::string("yellow"));

    insopt("metacache.expire", std::string("lastsync"));
    insopt("querycache.max", 100);
    insopt("querycache.expire", static_cast<long>(84600));
    insopt("wget.options", std::string("-rq -t3 -T15"));
    insopt("devaway.expire", static_cast<long>(84600));

    insopt("maxcol", static_cast<std::size_t>(78));

    insopt("localstatedir", std::string(std::string(LOCALSTATEDIR)+"/herdstat"));
    insopt("herds.xml", std::string());
    insopt("devaway.location", std::string(""));

    insopt("with-herd", std::string());
    insopt("with-maintainer", std::string());
    insopt("outfile", std::string("stdout"));
    insopt("outstream", &std::cout);

    insopt("highlights", std::string());

    insopt("action",
        static_cast<options_action_T>(action_unspecified));

    insopt("locale", std::string(std::locale::classic().name()));

    portage::config_T config;
    insopt("portage.config", config);
    insopt("portdir", config.portdir());
}

/* vim: set tw=80 sw=4 et : */
