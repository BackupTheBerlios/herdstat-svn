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

#include <iostream>
#include <string>
#include <iterator>
#include <algorithm>
#include <cstring>
#include <cassert>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>

#include "util.hh"

/*
 * Try to determine if the current directory is
 * a valid package directory.
 */

bool
portage::in_pkg_dir()
{
    const char *pwd = util::getcwd().c_str();
    DIR *dir = NULL;
    struct dirent *d = NULL;
    bool ebuild = false, filesdir = false;

    if (not (dir = opendir(pwd)))
	throw util::bad_fileobject_E(pwd);

    while ((d = readdir(dir)))
    {
	char *s = NULL;
	if ((s = std::strrchr(d->d_name, '.')))
	{
	    if (std::strcmp(++s, "ebuild") == 0)
		ebuild = true;
	}   
	else if (std::strcmp(d->d_name, "files") == 0)
	    filesdir = true;
    }

    closedir(dir);
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

const std::string
portage::ebuild_which(const std::string &pkg)
{
    std::string package(pkg);
    bool pfound = false, ofound = false;
    portage::versions_T versions;
    portage::config_T config;
    std::vector<std::string> overlays(config.overlays());
    const std::string portdir(config.portdir());

    /* search PORTDIR/category/package */
    if (pkg.find('/') == std::string::npos)
    {
        try
        {
            package = portage::find_package(portdir, pkg);
        }
        catch (const portage::nonexistent_pkg_E &e)
        {
            /* do nothing to prevent it being caught elsewhere */
        }
    }

    if (util::is_dir(portdir + "/" + package))
    {
        util::dir_T pkgdir(portdir + "/" + package);
        util::dir_T::iterator d;
        for (d = pkgdir.begin() ; d != pkgdir.end() ; ++d)
        {
            util::path_T::size_type pos = d->rfind(".ebuild");
            if (pos == util::path_T::npos)
                continue;

            pfound = true;
            assert(versions.insert(*d));
        }
    }

    /* check the overlay(s) too */
    std::vector<std::string>::iterator i;
    for (i = overlays.begin() ; i != overlays.end() ; ++i)
    {
        if (not util::is_dir(*i))
            continue;

        if (package.find('/') == std::string::npos)
        {
            try
            {
                package = portage::find_package(*i, package);
            }
            catch (const portage::nonexistent_pkg_E &e)
            {
                /* do nothing to prevent it being caught elsewhere */
            }
        }

        if (not util::is_dir(*i + "/" + package))
            continue;
        
        util::dir_T pkgdir(*i + "/" + package);
        util::dir_T::iterator d;
        for (d = pkgdir.begin() ; d != pkgdir.end() ; ++d)
        {
            util::path_T::size_type pos = d->rfind(".ebuild");
            if (pos == util::path_T::npos)
                continue;

            ofound = true;
            /* we don't assert this one because there may be dupes */
            versions.insert(*d);
        }
    }

    if (not pfound and not ofound)
        throw portage::nonexistent_pkg_E(pkg);

    return versions.back()->ebuild();
}

const std::string
portage::find_package(const std::string &portdir, const std::string &pkg)
{
    /* if category/package was specified, just check for existence */
    if ((pkg.find('/') != std::string::npos) and
        (util::is_dir(portdir + "/" + pkg)))
        return pkg;

    std::vector<std::string> pkgs;
    portage::categories_T categories;
    portage::categories_T::iterator c;
    for (c = categories.begin() ; c != categories.end() ; ++c)
    {
        /* was a category specified? only one possible */
        if (*c == pkg)
            return pkg;

        if (not util::is_dir(portdir + "/" + (*c)))
            continue;

        util::dir_T category(portdir + "/" + (*c));
        util::dir_T::iterator d;

        /* for each package in the category */
        for (d = category.begin() ; d != category.end() ; ++d)
            if (pkg == d->basename())
                pkgs.push_back(*c + "/" + pkg);
    }

    if (pkgs.size() > 1)
        throw ambiguous_pkg_E(pkgs);
    else if (pkgs.size() < 1)
        throw nonexistent_pkg_E(pkg);

    return pkgs.front();
}

/*
 * Given a HOMEPAGE string, do our best to perform variable substituion
 * in an attempt to make the URL readable.
 */

const std::string
portage::parse_homepage(const std::string &homepage, util::vars_T &vars)
{
    std::string h(homepage);

    /* any variables present? */
    if (h.find("${") != std::string::npos)
    {
        std::vector<std::string> v;
        std::vector<std::string>::iterator i;
        std::string::size_type lpos = 0;

        while (true)
        {
            std::string::size_type begin = h.find("${", lpos);
            if (begin == std::string::npos)
                break;

            std::string::size_type end = h.find("}", begin);
            if (end == std::string::npos)
                break;

            v.push_back(h.substr(begin + 2, end - (begin + 2)));
            lpos = ++end;
        }

        /* for each variable we found in $HOMEPAGE */
        for (i = v.begin() ; i != v.end() ; ++i)
        {
            std::string subst;
            std::string var("${"+(*i)+"}");

            std::string::size_type pos = h.find(var);
            if (pos == std::string::npos)
                continue;

            /* is that variable defined in the ebuild? */
            util::vars_T::iterator x = vars.find(*i);
            if (x != vars.end())
                subst = x->second; /* found it */
            else
            {
                /* is the variable part of the version ($P, $PN, $PV, etc) */
                portage::version_string_T version(vars.name());
                portage::version_string_T::iterator v = version.find(*i);
                if (v != version.end())
                    subst = v->second;
            }

            if (not subst.empty())
                h.replace(pos, var.length(), subst, 0, subst.length());
        }
    }

    return h;
}

/* vim: set tw=80 sw=4 et : */
