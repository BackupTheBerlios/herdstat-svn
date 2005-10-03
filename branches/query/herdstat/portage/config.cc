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

namespace portage {
/*** static members *********************************************************/
bool config::_init = false;
std::string config::_portdir;
std::vector<std::string> config::_overlays;
/****************************************************************************/
config::config()
{
    if (_init)
        return;

    /* read default config */
    this->read("/etc/make.globals");
    this->close();
    /* read make.conf overriding any defined settings */
    this->read("/etc/make.conf");
    this->close();

    iterator x;
    char *result = NULL;

    /* PORTDIR */
    if ((result = std::getenv("PORTDIR")))
        _portdir.assign(result);
    else if ((x = this->find("PORTDIR")) != this->end())
        _portdir.assign(x->second);
    
    /* default to /usr/portage */
    if (_portdir.empty())
        _portdir.assign("/usr/portage");

    /* PORTDIR_OVERLAY */
    if ((result = std::getenv("PORTDIR_OVERLAY")))
    {
        if (std::strlen(result) > 0)
            _overlays = util::split(result);
    }
    else if ((x = this->find("PORTDIR_OVERLAY")) != this->end())
    {
        if (not x->second.empty())
            _overlays = util::split(x->second);
    }

    _init = true;
}
/****************************************************************************/
} // namespace portage

/* vim: set tw=80 sw=4 et : */
