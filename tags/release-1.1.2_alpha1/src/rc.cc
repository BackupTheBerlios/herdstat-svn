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
    std::string s;

    if (not vars["colors"].empty())
    {
	s = vars["colors"];
	if (s == "no" or s == "false")
	    optset("color", bool, false);
	else if (s == "yes" or s == "true")
	    optset("color", bool, true);
    }
    if (not vars["label.color"].empty())
	optset("label.color", std::string, vars["label.color"]);
    if (not vars["highlight.color"].empty())
	optset("highlight.color", std::string, vars["highlight.color"]);
    if (not vars["qa"].empty())
    {
	s = vars["qa"];
	if (s == "no" or s == "false")
	    optset("qa", bool, false);
	else if (s == "yes" or s == "true")
	    optset("qa", bool, true);
    }
    if (not vars["herdsxml"].empty())
	optset("herds.xml", std::string, vars["herdsxml"]);
    if (not vars["gentoo.cvs"].empty())
        optset("gentoo.cvs", std::string, vars["gentoo.cvs"]);
    if (not vars["userinfo"].empty())
        optset("userinfo", std::string, vars["userinfo"]);
    if (not vars["use.devaway"].empty())
    {
	s = vars["use.devaway"];
	if (s == "no" or s == "false")
	    optset("devaway", bool, false);
	else if (s == "yes" or s == "true")
	    optset("devaway", bool, true);
    }
    if (not vars["devaway.expire"].empty())
	optset("devaway.expire", long,
	    std::strtol(vars["devaway.expire"].c_str(), NULL, 10));
    if (not vars["devaway.location"].empty())
        optset("devaway.location", std::string, vars["devaway.location"]);
    if (not vars["use.metacache"].empty())
    {
	s = vars["use.metacache"];
	if (s == "no" or s == "false")
	    optset("metacache", bool, false);
	else if (s == "yes" or s == "true")
	    optset("metacache", bool, true);
    }
    if (not vars["metacache.expire"].empty())
	optset("metacache.expire", std::string, vars["metacache.expire"]);
    if (not vars["use.querycache"].empty())
    {
	s = vars["use.querycache"];
	if (s == "no" or s == "false")
	    optset("querycache", bool, false);
	else if (s == "yes" or s == "true")
	    optset("querycache", bool, true);
    }
    if (not vars["querycache.max"].empty())
	optset("querycache.max", int,
	    std::atoi(vars["querycache.max"].c_str()));
    if (not vars["querycache.expire"].empty())
	optset("querycache.expire", long,
	    std::strtol(vars["querycache.expire"].c_str(), NULL, 10));
    if (not vars["wget.options"].empty())
	optset("wget.options", std::string, vars["wget.options"]);

    if (not vars["highlights"].empty())
        optset("highlights", std::string, vars["highlights"]);
}

/* vim: set tw=80 sw=4 et : */
