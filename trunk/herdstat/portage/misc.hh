/*
 * herdstat -- herdstat/portage/portage_misc.hh
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

#ifndef HAVE_PORTAGE_MISC_HH
#define HAVE_PORTAGE_MISC_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <herdstat/util/misc.hh>

//#define CATEGORIES "/profiles/categories"
//#define CATEGORIES_USER "/etc/portage/categories"

namespace portage
{
    /** Is the specified path a package directory?
     * @param p Path.
     * @returns A boolean value.
     */
    bool is_pkg_dir(const std::string &p);

    /** Are we inside a package directory?
     * @returns A boolean value.
     */
    inline bool in_pkg_dir() { return is_pkg_dir(util::getcwd()); }

    /** Is the specified path an ebuild?
     * @param p Path.
     * @returns A boolean value.
     */
    inline bool is_ebuild(const std::string &path)
    {
        return ( (path.length() > 7) and
                 (path.substr(path.length() - 7) == ".ebuild") );
    }

} // namespace portage

#endif

/* vim: set tw=80 sw=4 et : */
