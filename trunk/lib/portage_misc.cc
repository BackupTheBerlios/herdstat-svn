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

const char *
portage::ebuild_which(const std::string &portdir, const std::string &pkg)
{
    const util::path_T path(portdir + "/" + pkg);
    util::dir_T pkgdir(path);
    portage::versions_T versions;

    for (util::dir_T::iterator d = pkgdir.begin() ; d != pkgdir.end() ; ++d)
    {
        util::path_T::size_type pos = d->rfind(".ebuild");
        if (pos == util::path_T::npos)
            continue;

        versions.insert(new portage::version_string_T(*d));
    }

    return (path + "/" + (*versions.back())() + ".ebuild").c_str();
}

const std::string
portage::find_package(const std::string &portdir, const std::string &pkg)
{
    std::vector<std::string> pkgs;

    /* if category was specified, just check for existence */
    std::string::size_type pos = pkg.find('/');
    if (pos != std::string::npos)
    {
        if (util::is_dir(portdir + "/" + pkg))
            pkgs.push_back(pkg);
        return pkgs;
    }

    portage::categories_T categories;
    portage::categories_T::iterator c;
    for (c = categories.begin() ; c != categories.end() ; ++c)
    {
        /* was a category specified? only one possible */
        if (*c == pkg)
        {
            pkgs.push_back(*c);
            break;
        }

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
 * Given the path to an ebuild, return a vector comprised
 * of the version components (package name, version, revision).
 */

std::vector<std::string>
portage::get_version_components(const util::path_T &path)
{
    std::vector<std::string> components, parts;
    std::string::size_type pos;
    util::string ebuild(path.basename());

    /* chop .ebuild */
    if ((pos = ebuild.rfind(".ebuild")) != std::string::npos)
        ebuild = ebuild.substr(0, pos);

    if ((pos = ebuild.rfind("-r")) == std::string::npos)
        ebuild.append("-r0");

    parts = ebuild.split('-');

    /* if parts > 3, package name contains a '-' */
    if (parts.size() > 3)
    {
        /* reconstruct PN */
        std::string PN = parts.front();
	parts.erase(parts.begin());

        while (parts.size() >= 3)
        {
            PN += "-" + parts.front();
            parts.erase(parts.begin());
        }

        components.push_back(PN);
    }

    std::copy(parts.begin(), parts.end(), std::back_inserter(components));

    assert(components.size() == 3);

    return components;
}

std::map<std::string, std::string>
portage::get_version_map(const std::string &path)
{
    std::map<std::string, std::string> version;
    std::vector<std::string> components = portage::get_version_components(path);

    version["PN"] = components[0];
    version["PV"] = components[1];
    version["PR"] = components[2];

    version["P"]  = version["PN"] + "-" + version["PV"];
    version["PVR"] = version["PV"] + "-" + version["PR"];
    version["PF"] = version["PN"] + "-" + version["PVR"];

    return version;
}

/*
 * Given a HOMEPAGE string, do our best to perform variable substituion
 * in an attempt to make the URL readable.
 */

std::string
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
