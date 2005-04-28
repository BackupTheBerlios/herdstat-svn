/*
 * herdstat -- lib/portage_misc.cc
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

#include "misc.hh"
#include "file.hh"
#include "portage_misc.hh"

/*****************************************************************************
 * Current working directory a package directory?                            *
 *****************************************************************************/
bool portage::in_pkg_dir() { return portage::is_pkg_dir(util::getcwd()); }
/*****************************************************************************
 * Given path a package directory?                                           *
 *****************************************************************************/
bool
portage::is_pkg_dir(const util::path_T &path)
{
    if (not util::is_dir(path))
        return false;

    bool ebuild = false, filesdir = false;
    const util::dir_T dir(path);
    util::dir_T::const_iterator d;

    for (d = dir.begin() ; d != dir.end() ; ++d)
    {
        if (portage::is_ebuild(*d))
            ebuild = true;
        else if (d->basename() == "files")
            filesdir = true;
    }

    return (ebuild and filesdir);
}
/*****************************************************************************
 * Is the given path an ebuild?                                              *
 *****************************************************************************/
bool
portage::is_ebuild(const util::path_T &path)
{
    return ( (path.length() > 7) and
             (path.substr(path.length() - 7) == ".ebuild") );
}
/*****************************************************************************/

/* vim: set tw=80 sw=4 et : */
