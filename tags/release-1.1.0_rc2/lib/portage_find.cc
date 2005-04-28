/*
 * herdstat -- lib/portage_find.cc
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

#include "file.hh"
#include "portage_misc.hh"
#include "portage_version.hh"
#include "portage_exceptions.hh"
#include "portage_config.hh"
#include "portage_find.hh"

const util::path_T
portage::ebuild_which(portage::config_T &config,
                      const util::string &pkg,
                      bool overlays)
{
    util::string portdir, package;
    portage::versions_T versions;

    std::pair<util::string, util::string> p =
        portage::find_package(config, pkg, overlays);
    portdir = p.first;
    package = p.second;

    return portage::ebuild_which(portdir, package);
}

const util::path_T
portage::ebuild_which(const util::string &portdir, const util::string &pkg)
{
    util::string package(pkg);
    portage::versions_T versions;

    if (pkg.find('/') == util::string::npos)
        package = find_package_in(portdir, pkg);

    if (util::is_dir(portdir + "/" + package))
    {
        util::dir_T pkgdir(portdir + "/" + package);
        util::dir_T::iterator d;
        for (d = pkgdir.begin() ; d != pkgdir.end() ; ++d)
        {
            if (not portage::is_ebuild(*d))
                continue;

            assert(versions.insert(*d));
        }
    }

    if (versions.empty())
        throw portage::nonexistent_pkg_E(pkg);

    return versions.back()->ebuild();
}

const util::string
portage::find_package_in(const util::string &portdir, const util::string &pkg)
{
    /* if category/package was specified, just check for existence */
    if ((pkg.find('/') != util::string::npos) and
        (util::is_dir(portdir + "/" + pkg)))
        return pkg;

    std::vector<util::string> pkgs;
    const portage::categories_T categories;
    portage::categories_T::const_iterator c;
    for (c = categories.begin() ; c != categories.end() ; ++c)
    {
        /* was a category specified? only one possible */
        if (*c == pkg and util::is_dir(portdir + "/" + pkg))
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
 * Given a portdir and a regular expression object,
 * search for all packages matching the regular expression.
 */

std::vector<util::string>
portage::find_package_regex_in(const util::string &portdir, util::regex_T &regex)
{
    std::vector<util::string> matches;
    const portage::categories_T categories;
    portage::categories_T::const_iterator c;

    for (c = categories.begin() ; c != categories.end() ; ++c)
    {
        if (not util::is_dir(portdir + "/" + (*c)))
            continue;

        const util::dir_T category(portdir + "/" + (*c));
        util::dir_T::const_iterator d;

        for (d = category.begin() ; d != category.end() ; ++d)
        {
            util::path_T base(d->basename());
            if (base[0] == '.' or base == "CVS" or not util::is_dir(*d))
                continue;

            /* Does category/package or package match the regex? */
            if ((regex == base) or (regex == (*c + "/" + base)))
                matches.push_back(*c + "/" + base);
        }
    }

    return matches;
}

/*
 * Given a vector of overlays, call find_package_in()
 * for each one, searching for the package.  This function
 * is used soley by find_package().
 */

static std::pair<util::string, util::string>
search_overlays(const std::vector<util::string> &overlays,
                const util::string &pkg)
{
    std::pair<util::string, util::string> p;

    /* search overlays */
    std::vector<util::string>::const_iterator o;
    for (o = overlays.begin() ; o != overlays.end() ; ++o)
    {
        try
        {
            p.second = portage::find_package_in(*o, pkg);
            p.first  = *o;
        }
        catch (const portage::nonexistent_pkg_E)
        {
            continue;
        }
    }

    return p;
}

std::pair<util::string, util::string>
portage::find_package(portage::config_T &config,
                      const util::string &pkg,
                      bool do_overlays)
{
    util::string package;
    util::string portdir(config.portdir());
    const std::vector<util::string> overlays(config.overlays());
    std::pair<util::string, util::string> p;

    try
    {
        package = portage::find_package_in(portdir, pkg);

        if (do_overlays)
        {
            p = search_overlays(overlays, pkg);
            if (not p.second.empty())
            {
                portdir = p.first;
                package = p.second;
            }
        }

        p.first = portdir;
        p.second = package;
    }
    catch (const portage::nonexistent_pkg_E)
    {
        bool found = false;

        if (do_overlays)
        {
            p = search_overlays(overlays, pkg);
            if (not p.second.empty())
                found = true;
        }

        if (not found)
            throw;
    }

    return p;
}

static std::multimap<util::string, util::string>
search_overlays_regex(const std::vector<util::string> &overlays,
                      util::regex_T &regex)
{
    std::vector<util::string> result;
    std::vector<util::string>::iterator r;
    std::multimap<util::string, util::string> matches;
    std::multimap<util::string, util::string>::iterator m;

    /* search overlays */
    std::vector<util::string>::const_iterator o;
    for (o = overlays.begin() ; o != overlays.end() ; ++o)
    {
        try
        {
            result = portage::find_package_regex_in(*o, regex);
            for (r = result.begin() ; r != result.end() ; ++r)
                matches.insert(std::make_pair(*o, *r));
        }
        catch (const portage::nonexistent_pkg_E)
        { continue; }
    }

    return matches;
}

std::multimap<util::string, util::string>
portage::find_package_regex(portage::config_T &config,
                            util::regex_T &regex,
                            bool do_overlays)
{
    util::string portdir(config.portdir());
    const std::vector<util::string> overlays(config.overlays());
    std::vector<util::string> result;
    std::vector<util::string>::iterator r;
    std::multimap<util::string, util::string> matches;
    std::multimap<util::string, util::string>::iterator m;

    try
    {
        result = portage::find_package_regex_in(portdir, regex);
        for (r = result.begin() ; r != result.end() ; ++r)
            matches.insert(std::make_pair(portdir, *r));

        if (do_overlays)
        {
            const std::multimap<util::string, util::string> omatches =
                search_overlays_regex(overlays, regex);

            std::multimap<util::string, util::string>::const_iterator o;
            for (o = omatches.begin() ; o != omatches.end() ; ++o)
            {
                if (not o->second.empty())
                    matches.insert(std::make_pair(o->first, o->second));
            }
        }
    }
    catch (const portage::nonexistent_pkg_E)
    {
        bool found = false;

        if (do_overlays)
        {
            const std::multimap<util::string, util::string> omatches =
                search_overlays_regex(overlays, regex);

            std::multimap<util::string, util::string>::const_iterator o;
            for (o = omatches.begin() ; o != omatches.end() ; ++o)
            {
                if (not o->second.empty())
                {
                    found = true;
                    matches.insert(std::make_pair(o->first, o->second));
                }
            }   
        }

        if (not found)
            throw;
    }

    return matches;
}

/* vim: set tw=80 sw=4 et : */
