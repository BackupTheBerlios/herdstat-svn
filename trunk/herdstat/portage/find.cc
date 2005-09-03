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

#include <herdstat/util/file.hh>
#include <herdstat/portage/misc.hh>
#include <herdstat/portage/version.hh>
#include <herdstat/portage/exceptions.hh>
#include <herdstat/portage/config.hh>
#include <herdstat/portage/find.hh>

/*****************************************************************************
 * Search the specified portdir for the latest ebuild of the specified pkg.  *
 *****************************************************************************/
const std::string
portage::ebuild_which(const std::string &portdir,
                      const std::string &pkg,
                      util::timer_T *timer,
                      const std::vector<std::string> &pkgcache)
{
    std::string package(pkg);
    portage::versions_T versions;

    if (timer and not timer->is_running())
        timer->start();

    /* find category if missing */
    if (pkg.find('/') == std::string::npos)
        package = find_package_in(portdir, pkg, timer, pkgcache);

    if (util::is_dir(portdir + "/" + package))
        versions.assign(portdir + "/" + package);

    if (timer)
        timer->stop();

    if (versions.empty())
        throw portage::NonExistentPkg(pkg);

    return versions.back()->ebuild();
}
/*****************************************************************************
 * Overloaded ebuild_which() that takes a portage::config_T object and       *
 * searches overlays if overlays is true.                                    *
 *****************************************************************************/
const std::string
portage::ebuild_which(const portage::config_T &config,
                      const std::string &pkg,
                      bool overlays, util::timer_T *timer,
                      const std::vector<std::string> &pkgcache)
{
    std::pair<std::string, std::string> p =
        portage::find_package(config, pkg, overlays, timer, pkgcache);

    /*
     * Check if the portdir returned is the real portdir.  If not we need to
     * check if the version in the overlay is actually greater than the version
     * in portage.
     */

    std::string ebuild;
    const std::string portdir(config.portdir());
    if (p.first != portdir)
    {
        std::string ebuild1, ebuild2;

        try
        {
            ebuild1 = ebuild_which(portdir, p.second, timer, pkgcache);
            ebuild2 = ebuild_which(p.first, p.second, timer, pkgcache);
        }
        catch (const portage::NonExistentPkg &e)
        {
            ebuild2 = ebuild_which(p.first, p.second, timer, pkgcache);
        }

        if (ebuild1.empty())
            ebuild = ebuild2;
        else
        {
            /* pick which one is greater */
            portage::version_string_T ver1(ebuild1);
            portage::version_string_T ver2(ebuild2);

            if (ver1 > ver2)
                ebuild = ver1.ebuild();
            else /* ver1 <= ver2 */
                ebuild = ver2.ebuild();
        }
    }
    else
        ebuild = ebuild_which(p.first, p.second, timer);

    return ebuild;
}
/*****************************************************************************
 * Search the specified portdir for the specified package (either just in    *
 * package form or category/package form).                                   *
 *****************************************************************************/
const std::string
portage::find_package_in(const std::string &portdir,
                         const std::string &pkg,
                         util::timer_T *timer,
                         const std::vector<std::string> &pkgcache)
{
    if (timer and not timer->is_running())
        timer->start();

    /* if category/package was specified, just check and
     * make sure it's a valid package directory */
    if (pkg.find('/') != std::string::npos)
    {
        if (is_pkg_dir(portdir + "/" + pkg))
        {
            if (timer) timer->stop();
            return pkg;
        }
        
        throw portage::NonExistentPkg(pkg);
    }
    else if (util::is_dir(portdir + "/" + pkg))
    {
        if (timer) timer->stop();
        return pkg;
    }
    
    std::vector<std::string> pkgs;

    if (pkgcache.empty())
    {
        const categories_T categories;
        categories_T::const_iterator c = categories.begin();
        categories_T::const_iterator ce = categories.end();
        for ( ; c != ce ; ++c)
        {
            /* was a category specified? only one possible */
            if (*c == pkg)
            {
                if (util::is_dir(portdir + "/" + pkg))
                {
                    if (timer) timer->stop();
                    return pkg;
                }
            
                throw portage::NonExistentPkg(pkg);
            }

            if (not util::is_dir(portdir + "/" + (*c)))
                continue;

            const util::dir_T category(portdir + "/" + (*c));
            util::dir_T::const_iterator d = category.begin(),
                                        e = category.end();

            /* for each package in the category */
            for (; d != e ; ++d)
            {
                if ((pkg == util::basename(*d)) and
                    is_pkg_dir(portdir + "/" + (*c) + "/" + pkg))
                    pkgs.push_back(*c + "/" + pkg);
            }
        }
    }
    else
    {
        std::vector<std::string>::const_iterator i = pkgcache.begin(),
                                                  e = pkgcache.end();
        for (; i != e ; ++i)
        {
            std::string::size_type pos = i->find('/');
            if (pos != std::string::npos)
            {
                if (i->substr(pos+1) == pkg and
                    is_pkg_dir(portdir + "/" + (*i)))
                    pkgs.push_back(*i);
            }
        }
    }

    if (timer)
        timer->stop();

    if (pkgs.size() > 1)
        throw AmbiguousPkg(pkgs);
    else if (pkgs.size() < 1)
        throw NonExistentPkg(pkg);

    return pkgs.front();
}
/*****************************************************************************
 * Given a portdir and a regular expression object,                          *
 * search for all packages matching the regular expression.                  *
 *****************************************************************************/
std::vector<std::string>
portage::find_package_regex_in(const std::string &portdir,
                               const util::regex_T &regex,
                               util::timer_T *timer,
                               const std::vector<std::string> &pkgcache)
{
    /* if it looks like a category was specified, and no
     * regex-like metacharacters are present, only search
     * the category */
    bool knowncat = false, onlyonce = false;
    std::string cat;
    std::string::size_type pos = regex().find('/');
    if (pos != std::string::npos)
    {
        cat = regex().substr(0, pos);
        if (not cat.empty())
        {
            knowncat = ((cat.find_first_of("`~!@#$%^&*()+=[]{}<>,.;:'\"") ==
                    std::string::npos) and (util::is_dir(portdir+"/"+cat)));
        }
    }

    if (timer and not timer->is_running())
        timer->start();

    std::vector<std::string> matches;

    if (pkgcache.empty())
    {
        const categories_T categories;
        categories_T::const_iterator c = categories.begin(),
                                     ce = categories.end();

        for (; c != ce ; ++c)
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
            util::dir_T::const_iterator d = category.begin(),
                                        e = category.end();

            for (; d != e ; ++d)
            {
                std::string base(util::basename(*d));
                if (base[0] == '.' or base == "CVS" or not util::is_dir(*d))
                    continue;

                /* Does category/package or package match the regex? */
                if ((regex == base) or (regex == (*c + "/" + base)))
                    matches.push_back(*c + "/" + base);
            }

            if (onlyonce)
                break;
        }
    }
    else
    {
        std::vector<std::string>::const_iterator i = pkgcache.begin(),
                                                  e = pkgcache.end();
        for (; i != e ; ++i)
        {
            std::string::size_type pos = i->find('/');
            assert(pos != std::string::npos);

            std::string p(i->substr(pos+1));
            if ((knowncat and i->substr(0, pos) != cat) or
                (p == "metadata.xml") or (p[0] == '.'))
                continue;

            if (util::is_dir(portdir+"/"+(*i)) and
                ((regex == p) or (regex == *i)))
                matches.push_back(*i);
        }
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
static std::pair<std::string, std::string>
search_overlays(const std::vector<std::string> &overlays,
                const std::string &pkg,
                util::timer_T *timer,
                const std::vector<std::string> &pkgcache)
{
    std::pair<std::string, std::string> p;

    /* search overlays */
    std::vector<std::string>::const_iterator o = overlays.begin(),
                                              e = overlays.end();
    for (; o != e ; ++o)
    {
        try
        {
            p.second = portage::find_package_in(*o, pkg, timer, pkgcache);
            p.first  = *o;
        }
        catch (const portage::NonExistentPkg)
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
std::pair<std::string, std::string>
portage::find_package(const portage::config_T &config,
                      const std::string &pkg,
                      bool do_overlays,
                      util::timer_T *timer,
                      const std::vector<std::string> &pkgcache)
{
    std::string package;
    std::string portdir(config.portdir());
    const std::vector<std::string> overlays(config.overlays());
    std::pair<std::string, std::string> p;

    try
    {
        package = portage::find_package_in(portdir, pkg, timer, pkgcache);

        if (do_overlays)
        {
            p = search_overlays(overlays, pkg, timer, pkgcache);
            if (not p.second.empty())
            {
                portdir = p.first;
                package = p.second;
            }
        }

        p.first = portdir;
        p.second = package;
    }
    catch (const portage::NonExistentPkg)
    {
        bool found = false;

        if (do_overlays)
        {
            p = search_overlays(overlays, pkg, timer, pkgcache);
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
static std::multimap<std::string, std::string>
search_overlays_regex(const std::vector<std::string> &overlays,
                      const util::regex_T &regex,
                      util::timer_T *timer,
                      const std::vector<std::string> &pkgcache)
{
    std::vector<std::string> result;
    std::vector<std::string>::iterator r, re;
    std::multimap<std::string, std::string> matches;
    std::multimap<std::string, std::string>::iterator m;

    /* search overlays */
    std::vector<std::string>::const_iterator o = overlays.begin(),
                                              e = overlays.end();
    for (; o != e ; ++o)
    {
        try
        {
            result = portage::find_package_regex_in(*o, regex, timer, pkgcache);
            r = result.begin(), re = result.end();
            for (; r != re ; ++r)
                matches.insert(std::make_pair(*o, *r));
        }
        catch (const portage::NonExistentPkg)
        { continue; }
    }

    return matches;
}
/*****************************************************************************
 * Regular expression version of find_package() that takes a regex instead   *
 * a package std::string.                                                         *
 *****************************************************************************/
std::multimap<std::string, std::string>
portage::find_package_regex(const portage::config_T &config,
                            const util::regex_T &regex,
                            bool do_overlays,
                            util::timer_T *timer,
                            const std::vector<std::string> &pkgcache)
{
    std::string portdir(config.portdir());
    const std::vector<std::string> overlays(config.overlays());
    std::vector<std::string> result;
    std::vector<std::string>::iterator r, re;
    std::multimap<std::string, std::string> matches;
    std::multimap<std::string, std::string>::iterator m;

    try
    {
        result = portage::find_package_regex_in(portdir, regex, timer, pkgcache);
        r = result.begin(), re = result.end();
        for (; r != re ; ++r)
            matches.insert(std::make_pair(portdir, *r));

        if (do_overlays)
        {
            const std::multimap<std::string, std::string> omatches =
                search_overlays_regex(overlays, regex, timer, pkgcache);

            std::multimap<std::string, std::string>::const_iterator
                o = omatches.begin(), oe = omatches.end();
            for (; o != oe ; ++o)
                if (not o->second.empty())
                    matches.insert(*o);
        }
    }
    catch (const portage::NonExistentPkg)
    {
        bool found = false;

        if (do_overlays)
        {
            const std::multimap<std::string, std::string> omatches =
                search_overlays_regex(overlays, regex, timer, pkgcache);

            std::multimap<std::string, std::string>::const_iterator
                o = omatches.begin(), oe = omatches.end();
            for (; o != oe ; ++o)
            {
                if (not o->second.empty())
                {
                    found = true;
                    matches.insert(*o);
                }
            }   
        }

        if (not found)
            throw;
    }

    return matches;
}

/* vim: set tw=80 sw=4 et : */
