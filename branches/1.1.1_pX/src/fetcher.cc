/*
 * herdstat -- src/fetcher.cc
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

#include "fetcher.hh"

int
fetcher_T::fetch(const std::string &url, const std::string &file)
{
    const char *dir = util::dirname(file);
    if (access(dir, W_OK) != 0)
        throw util::bad_fileobject_E(dir);

    std::string opts(optget("wget.options", std::string));
    
    /* try to be somewhat safe and search for occurrences of
     * ';' and '&&' from wget options just in case... */
    std::string::size_type pos = opts.find(';');
    if (pos != std::string::npos)
        opts = opts.substr(0, pos);
    if ((pos = opts.find("&&")) != std::string::npos)
        opts = opts.substr(0, pos);

    std::string cmd(util::sprintf("%s %s -O %s '%s'", WGET, opts.c_str(),
        file.c_str(), url.c_str()));

    debug_msg("executing '%s'", cmd.c_str());
    return std::system(cmd.c_str());
}

/* vim: set tw=80 sw=4 et : */
