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

#include <string>
#include <cstring>
#include <cassert>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>

#include "util.hh"

/*
 * Determine whether or not the current directory is
 * a valid package directory.  Must have a Manifest,
 * at least one ebuild, and a files directory.
 */

bool
portage::in_pkg_dir()
{
//    const char *pwd = util::getcwd().c_str();
//    DIR *dir = NULL;
//    struct dirent *d = NULL;
//    bool ebuild = false, filesdir = false;

//    if (not (dir = opendir(pwd)))
//        throw util::bad_fileobject_E(pwd);

//    while ((d = readdir(dir)))
//    {
//        char *s = NULL;
//        if ((s = std::strrchr(d->d_name, '.')))
//        {
//            if (std::strcmp(++s, "ebuild") == 0)
//                ebuild = true;
//        }   
//        else if (std::strcmp(d->d_name, "files") == 0)
//            filesdir = true;
//    }

//    closedir(dir);
//    return (ebuild and filesdir);

    return portage::is_pkg_dir(util::getcwd());
}

/*
 * Is the given path a package directory?
 */

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

/*
 * Is the given path an ebuild?
 */

bool
portage::is_ebuild(const util::path_T &path)
{
    return ( (path.length() > 7) and
             (path.substr(path.length() - 7) == ".ebuild") );
}

/* vim: set tw=80 sw=4 et : */
