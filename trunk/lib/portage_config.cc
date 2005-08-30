/*
 * herdstat -- lib/portage_config.cc
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

#include "portage_config.hh"

/*
 * Determine PORTDIR
 */

const char *
portage::portdir()
{
    portage::config_T config;
    std::string portdir = config["PORTDIR"];

    /* environment overrides all */
    char *result = std::getenv("PORTDIR");
    if (result)
	portdir = result;

    return (portdir.empty() ? "/usr/portage" : portdir.c_str());
}

/*
 * Determine PORTDIR
 */

const portage::config_T::string_type
portage::config_T::portdir() const
{
    std::string portdir = (*this)["PORTDIR"];

    /* environment overrides all */
    char *result = std::getenv("PORTDIR");
    if (result)
	portdir = result;

    return (portdir.empty() ? "/usr/portage" : portdir);
}

/*
 * Determine PORTDIR_OVERLAY
 */

const std::vector<portage::config_T::string_type>
portage::config_T::overlays() const
{
    string_type overlays = (*this)["PORTDIR_OVERLAY"];

    char *result = std::getenv("PORTDIR_OVERLAY");
    if (result)
        overlays = result;

    return util::split(overlays);
}

/* vim: set tw=80 sw=4 et : */
