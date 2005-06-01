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

/*****************************************************************************
 * Search the specified portdir for the latest ebuild of the specified pkg.  *
 *****************************************************************************/
const util::path_T
portage::ebuild_which(const util::string &portdir, const util::string &pkg,
                      util::timer_T *timer)
{
    util::string package(pkg);
    portage::versions_T versions;

    if (timer and not timer->is_running())
        timer->start();

    /* find category if missing */
    if (pkg.find('/') == util::string::npos)
        package = find_package_in(portdir, pkg, timer);

    if (util::is_dir(portdir + "/" + package))
        versions.assign(portdir + "/" + package);

    if (timer)
        timer->stop();

    if (versions.empty())
        throw portage::nonexistent_pkg_E(pkg);

    return versions.back()->ebuild();
}
/*****************************************************************************
 * Overloaded ebuild_which() that takes a portage::config_T object and       *
 * searches overlays if overlays is true.                                    *
 *****************************************************************************/
const util::path_T
portage::ebuild_which(const portage::config_T &config, const util::string &pkg,
                      bool overlays, util::timer_T *timer)
{
    std::pair<util::string, util::string> p =
        portage::find_package(config, pkg, overlays, timer);

    /*
     * Check if the portdir returned is the real portdir.  If not we need to
     * check if the version in the overlay is actually greater than the version
     * in portage.
     */

    util::path_T ebuild;
    const util::string portdir(config.portdir());
    if (p.first != portdir)
    {
        portage::version_string_T ver1(ebuild_which(portdir, p.second, timer));
        portage::version_string_T ver2(ebuild_which(p.first, p.second, timer));

        if (ver1 > ver2)
            ebuild = ver1.ebuild();
        else /* ver1 <= ver2 */
            ebuild = ver2.ebuild();
    }
    else
        ebuild = ebuild_which(p.first, p.second, timer);

    return ebuild;
}
/*****************************************************************************
 * Search the specified portdir for the specified package (either just in    *
 * package form or category/package form).                                   *
 *****************************************************************************/
const util::string
portage::find_package_in(const util::string &portdir, const util::string &pkg,
                         util::timer_T *timer)
{
    if (timer and not timer->is_running())
        timer->start();

    /* if category/package was specified, just check and
     * make sure it's a valid package directory */
    if (pkg.find('/') != util::string::npos)
    {
        if (portage::is_pkg_dir(portdir + "/" + pkg))
        {
            if (timer) timer->stop();
            return pkg;
        }
        
        throw portage::nonexistent_pkg_E(pkg);
    }

    std::vector<util::string> pkgs;
    const portage::categories_T categories;
    portage::categories_T::const_iterator c;
    for (c = categories.begin() ; c != categories.end() ; ++c)
    {
        /* was a category specified? only one possible */
        if (*c == pkg)
        {
            if (util::is_dir(portdir + "/" + pkg))
            {
                if (timer) timer->stop();
                return pkg;
            }
            
            throw portage::nonexistent_pkg_E(pkg);
        }

        if (not util::is_dir(portdir + "/" + (*c)))
            continue;

        const util::dir_T category(portdir + "/" + (*c));
        util::dir_T::const_iterator d;

        /* for each package in the category */
        for (d = category.begin() ; d != category.end() ; ++d)
            if (pkg == d->basename())
                pkgs.push_back(*c + "/" + pkg);
    }

    if (timer)
        timer->stop();

    if (pkgs.size() > 1)
        throw ambiguous_pkg_E(pkgs);
    else if (pkgs.size() < 1)
        throw nonexistent_pkg_E(pkg);

    return pkgs.front();
}
/*****************************************************************************
 * Given a portdir and a regular expression object,                          *
 * search for all packages matching the regular expression.                  *
 *****************************************************************************/
std::vector<util::string>
portage::find_package_regex_in(const util::string &portdir,
                               const util::regex_T &regex,
                               util::timer_T *timer)
{
    /* if it looks like a category was specified, and no
     * regex-like metacharacters are present, only search
     * the category */
    bool knowncat = false, onlyonce = false;
    util::string cat;
    util::string::size_type pos = regex().find('/');
    if (pos != util::string::npos)
    {
        cat = regex().substr(0, pos);
        if (not cat.empty())
        {
            knowncat = ((cat.find_first_of("`~!@#$%^&*()+=[]{}<>,.;:'\"") ==
                    util::string::npos) and (util::is_dir(portdir+"/"+cat)));
        }
    }

    if (timer and not timer->is_running())
        timer->start();

    std::vector<util::string> matches;
    const portage::categories_T categories;
    portage::categories_T::const_iterator c;

    for (c = categories.begin() ; c != categories.end() ; ++c)
    {
        if (not util::is_dir(portdir + "/" + (*c)))
            continue;

        if (knowncat)
        {
            if (*c == cat)
                onlyonce = true;
            else
                continue;
        }

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

        if (onlyonce)
            break;
    }

    if (timer)
        timer->stop();

    return matches;
}
/*****************************************************************************
 * Given a vector of overlays, call find_package_in() for each one,          *
 * searching for the package.  This function is used soley by                *
 * find_package().                                                           *
 *****************************************************************************/
static std::pair<util::string, util::string>
search_overlays(const std::vector<util::string> &overlays,
                const util::string &pkg,
                util::timer_T *timer)
{
    std::pair<util::string, util::string> p;

    /* search overlays */
    std::vector<util::string>::const_iterator o;
    for (o = overlays.begin() ; o != overlays.end() ; ++o)
    {
        try
        {
            p.second = portage::find_package_in(*o, pkg, timer);
            p.first  = *o;
        }
        catch (const portage::nonexistent_pkg_E)
        {
            continue;
        }
    }

    return p;
}
/*****************************************************************************
 * Call find_package_in() for the real portdir, and call search_overlays()   *
 * to search all the overlays if do_overlays == true.                        *
 *****************************************************************************/
std::pair<util::string, util::string>
portage::find_package(const portage::config_T &config,
                      const util::string &pkg,
                      bool do_overlays,
                      util::timer_T *timer)
{
    util::string package;
    util::string portdir(config.portdir());
    const std::vector<util::string> overlays(config.overlays());
    std::pair<util::string, util::string> p;

    try
    {
        package = portage::find_package_in(portdir, pkg, timer);

        if (do_overlays)
        {
            p = search_overlays(overlays, pkg, timer);
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
            p = search_overlays(overlays, pkg, timer);
            if (not p.second.empty())
                found = true;
        }

        if (not found)
            throw;
    }

    return p;
}
/*****************************************************************************
 * Regular expression version of search_overlays().                          *
 *****************************************************************************/
static std::multimap<util::string, util::string>
search_overlays_regex(const std::vector<util::string> &overlays,
                      const util::regex_T &regex,
                      util::timer_T *timer)
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
            result = portage::find_package_regex_in(*o, regex, timer);
            for (r = result.begin() ; r != result.end() ; ++r)
                matches.insert(std::make_pair(*o, *r));
        }
        catch (const portage::nonexistent_pkg_E)
        { continue; }
    }

    return matches;
}
/*****************************************************************************
 * Regular expression version of find_package() that takes a regex instead   *
 * a package string.                                                         *
 *****************************************************************************/
std::multimap<util::string, util::string>
portage::find_package_regex(const portage::config_T &config,
                            const util::regex_T &regex,
                            bool do_overlays,
                            util::timer_T *timer)
{
    util::string portdir(config.portdir());
    const std::vector<util::string> overlays(config.overlays());
    std::vector<util::string> result;
    std::vector<util::string>::iterator r;
    std::multimap<util::string, util::string> matches;
    std::multimap<util::string, util::string>::iterator m;

    try
    {
        result = portage::find_package_regex_in(portdir, regex, timer);
        for (r = result.begin() ; r != result.end() ; ++r)
            matches.insert(std::make_pair(portdir, *r));

        if (do_overlays)
        {
            const std::multimap<util::string, util::string> omatches =
                search_overlays_regex(overlays, regex, timer);

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
                search_overlays_regex(overlays, regex, timer);

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
