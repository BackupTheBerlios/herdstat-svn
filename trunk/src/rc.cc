/*
 * herdstat -- src/rc.cc
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

#include <string>
#include <herdstat/util/string.hh>
#include "common.hh"
#include "rc.hh"

#define HERDSTATRC_GLOBAL   SYSCONFDIR"/"PACKAGE"rc"
#define HERDSTATRC_LOCAL    "/."PACKAGE"rc"

rc_T::rc_T()
{
    if (util::is_file(HERDSTATRC_GLOBAL))
    {
        debug_msg("Reading configuration file %s", HERDSTATRC_GLOBAL);

        vars.read(HERDSTATRC_GLOBAL);
        set_options();
        vars.close();
    }

    char *result = std::getenv("HOME");
    if (result)
    {
        std::string path(std::string(result) + HERDSTATRC_LOCAL);
        if (not util::is_file(path))
            return;

        debug_msg("Reading configuration file %s", path.c_str());

        vars.read(path);
        set_options();
        vars.close();
    }
}

void
rc_T::set_options()
{
    if (not vars["colors"].empty())
        options::set_color(util::destringify<bool>(vars["colors"]));
    if (not vars["label.color"].empty())
	options::set_labelcolor(vars["label.color"]);
    if (not vars["highlight.color"].empty())
	options::set_hlcolor(vars["highlight.color"]);
    if (not vars["qa"].empty())
        options::set_qa(util::destringify<bool>(vars["qa"]));
    if (not vars["herdsxml"].empty())
	options::set_herdsxml(vars["herdsxml"]);
    if (not vars["gentoo.cvs"].empty())
        options::set_cvsdir(vars["gentoo.cvs"]);
    if (not vars["userinfo"].empty())
        options::set_userinfoxml(vars["userinfo"]);
    if (not vars["use.devaway"].empty())
        options::set_devaway(util::destringify<bool>(vars["use.devaway"]));
    if (not vars["devaway.expire"].empty())
        options::set_devaway_expire(util::destringify<long>(vars["devaway.expire"]));
    if (not vars["devaway.location"].empty())
        options::set_devawayxml(vars["devaway.location"]);
    if (not vars["use.metacache"].empty())
        options::set_metacache(util::destringify<bool>(vars["use.metacache"]));
    if (not vars["metacache.expire"].empty())
	options::set_metacache_expire(vars["metacache.expire"]);
    if (not vars["use.querycache"].empty())
        options::set_querycache(util::destringify<bool>(vars["use.querycache"]));
    if (not vars["querycache.max"].empty())
        options::set_querycache_max(util::destringify<int>(vars["querycache.max"]));
    if (not vars["querycache.expire"].empty())
        options::set_querycache_expire(util::destringify<long>(vars["querycache.expire"]));
    if (not vars["wget.options"].empty())
	options::set_wget_options(vars["wget.options"]);
    if (not vars["highlights"].empty())
        options::set_highlights(vars["highlights"]);
}

/* vim: set tw=80 sw=4 et : */
