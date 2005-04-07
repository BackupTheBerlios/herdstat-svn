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
 * Do our best to guess the latest ebuild of the specified
 * package. TODO: write an actual version parsing class since
 * this often produces incorrect results.
 */

const char *
portage::ebuild_which(const std::string &portdir, const std::string &pkg)
{
    DIR *dir = NULL;
    struct dirent *d = NULL;
    const std::string path = portdir + "/" + pkg;
    std::vector<std::string> ebuilds;
    std::vector<std::string>::iterator e;

    /* open package directory */
    if (not (dir = opendir(path.c_str())))
        throw util::bad_fileobject_E(path);

    /* read package directory looking for ebuilds */
    while ((d = readdir(dir)))
    {
	char *s = NULL;
	if ((s = std::strrchr(d->d_name, '.')))
	    if (std::strcmp(++s, "ebuild") == 0)
		ebuilds.push_back(path + "/" + d->d_name);
    }

    closedir(dir);

    if (ebuilds.empty())
	return "";

    std::sort(ebuilds.begin(), ebuilds.end());
    return ebuilds.back().c_str();
}

/*
 * Given the path to an ebuild, return a vector comprised
 * of the version components (package name, version, revision).
 */

std::vector<std::string>
portage::get_version_components(const std::string &path)
{
    std::vector<std::string> components, parts;
    std::string::size_type pos;
    std::string ebuild(util::basename(path));

    /* chop .ebuild */
    if ((pos = ebuild.rfind(".ebuild")) != std::string::npos)
        ebuild = ebuild.substr(0, pos);

    if ((pos = ebuild.rfind("-r")) == std::string::npos)
        ebuild.append("-r0");

    parts = util::split(ebuild, '-');

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

    if (h.find('$') != std::string::npos)
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

        for (i = v.begin() ; i != v.end() ; ++i)
        {
            std::string subst;
            std::string var("${"+(*i)+"}");

            std::string::size_type pos = h.find(var);
            if (pos == std::string::npos)
                continue;

            util::vars_T::iterator x = vars.find(*i);
            if (x != vars.end())
                subst = x->second;
            else
            {
                /* TODO: make a version class that does this */
                std::map<std::string, std::string> version =
                    portage::get_version_map(vars.name());
                std::map<std::string, std::string>::iterator y = version.find(*i);
                if (y != version.end())
                    subst = y->second;
            }

            if (not subst.empty())
                h.replace(pos, var.length(), subst, 0, subst.length());
        }
    }

    return h;
}

/* vim: set tw=80 sw=4 et : */
