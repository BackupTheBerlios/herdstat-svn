/*
 * herdstat -- herdstat/portage/portage_config.cc
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

#include <herdstat/util/string.hh>
#include <herdstat/portage/config.hh>

/*
 * Determine PORTDIR
 */

const char *
portage::portdir()
{
    std::string portdir;

    /* environment overrides all */
    char *result = std::getenv("PORTDIR");
    if (result)
	    portdir.assign(result);
    else
    {
        config_T config;
        portdir.assign(config["PORTDIR"]);
    }

    return (portdir.empty() ? "/usr/portage" : portdir.c_str());
}

/*
 * Determine PORTDIR
 */

const std::string
portage::config_T::portdir() const
{
    const_iterator i;
    std::string portdir;

    char *result = std::getenv("PORTDIR");
    if (result)
        portdir.assign(result);
    else if ((i = this->find("PORTDIR")) != this->end())
        portdir.assign(i->second);

    return (portdir.empty() ? "/usr/portage" : portdir);
}

/*
 * Determine PORTDIR_OVERLAY
 */

const std::vector<std::string>
portage::config_T::overlays() const
{
    const_iterator i;
    std::string overlays;

    char *result = std::getenv("PORTDIR_OVERLAY");
    if (result)
        overlays.assign(result);
    else if ((i = this->find("PORTDIR_OVERLAY")) != this->end())
        overlays.assign(i->second);

    return util::split(overlays);
}

/* vim: set tw=80 sw=4 et : */
