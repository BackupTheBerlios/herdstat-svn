/*
 * herdstat -- herdstat/portage/package_list.cc
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

#include <algorithm>
#include <functional>
#include <iterator>

#include <herdstat/util/algorithm.hh>
#include <herdstat/util/file.hh>
#include <herdstat/portage/misc.hh>
#include <herdstat/portage/config.hh>
#include <herdstat/portage/categories.hh>
#include <herdstat/portage/package_list.hh>

namespace portage {
/*** static members *********************************************************/
bool PackageList::_init = false;
PackageList::container_type PackageList::_pkgs;
/****************************************************************************/
PackageList::PackageList()
    : _portdir(config::portdir()), _overlays(config::overlays())
{
}
/****************************************************************************/
PackageList::PackageList(const std::string& portdir,
                         const std::vector<std::string>& overlays)
    : _portdir(portdir), _overlays(overlays)
{
}
/****************************************************************************/
void
PackageList::fill()
{
    if (_init)
        return;

    std::string path;
    const Categories categories(_portdir);
    Categories::const_iterator ci, cend;

    /* we can only use the estimate here */
    _pkgs.reserve(PKGLIST_RESERVE);

    /* search portdir */
    for (ci = categories.begin(), cend = categories.end() ; ci != cend ; ++ci)
    {
        path.assign(_portdir+"/"+(*ci));
        if (not util::is_dir(path))
            continue;

        /* for each pkg in category, insert "cat/pkg" into _pkgs */
        const util::dir_T cat(path);
        std::transform(cat.begin(), cat.end(), std::back_inserter(_pkgs),
            GetPkgFromPath());
    }

    /* search overlays, if any */
    if (not _overlays.empty())
    {
        std::vector<std::string>::const_iterator oi, oend = _overlays.end();
        for (ci = categories.begin() ; ci != cend ; ++ci)
        {
            for (oi = _overlays.begin() ; oi != oend ; ++oi)
            {
                path.assign(*oi+"/"+(*ci));
                if (not util::is_dir(path))
                    continue;

                /* for each pkg in category, insert "cat/pkg" into _pkgs */
                const util::dir_T cat(path);
                std::transform(cat.begin(), cat.end(),
                    std::back_inserter(_pkgs), GetPkgFromPath());
            }
        }
    }

    std::sort(_pkgs.begin(), _pkgs.end());

    /* container may contain duplicates if overlays were searched */
    if (not _overlays.empty())
        _pkgs.erase(std::unique(_pkgs.begin(), _pkgs.end()), _pkgs.end());

    _init = true;
}
/****************************************************************************/
} // namespace portage

/* vim: set tw=80 sw=4 et : */
