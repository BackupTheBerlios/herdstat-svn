/*
 * herdstat -- herdstat/portage/portage_misc.cc
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

#include <herdstat/util/regex.hh>
#include <herdstat/util/file.hh>
#include <herdstat/portage/exceptions.hh>
#include <herdstat/portage/misc.hh>

namespace portage {
/*****************************************************************************
 * Given path a package directory?                                           *
 *****************************************************************************/
bool
is_pkg_dir(const std::string &path)
{
    if (not util::is_dir(path))
        return false;

    const util::Regex regex("\\.ebuild$");
    const util::dir_T dir(path);
    return dir.find(regex) != dir.end();
}
/*****************************************************************************/
} // namespace portage

/* vim: set tw=80 sw=4 et : */
