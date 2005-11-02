/*
 * herdstat -- src/action/versions.cc
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

#include <herdstat/portage/exceptions.hh>
#include <herdstat/portage/misc.hh>
#include <herdstat/portage/find.hh>
#include <herdstat/portage/version.hh>

#include "common.hh"
#include "overlaydisplay.hh"
#include "action/versions.hh"

using namespace herdstat;
using namespace gui;

bool
VersionsActionHandler::allow_empty_query() const
{
    return true;
}

const char * const
VersionsActionHandler::id() const
{
    return "versions";
}

const char * const
VersionsActionHandler::desc() const
{
    return "Show version information for the given package(s).";
}

const char * const
VersionsActionHandler::usage() const
{
    return "versions <package(s)>";
}

Tab *
VersionsActionHandler::createTab(WidgetFactory *widgetFactory)
{
    Tab *tab = widgetFactory->createTab();
    tab->set_title(this->id());

    return tab;
}

void
VersionsActionHandler::operator()(const Query& qq,
                                  QueryResults * const results)
{
    Query query(qq);
    OverlayDisplay od(results);
    std::string dir;
    bool pwd = false;

    if (query.all())
    {
        results->add("versions action handler doesn't support the 'all' target.");
        throw ActionException();
    }

    if (query.empty())
    {
        /* are we in a package's directory? */
        if (not portage::in_pkg_dir())
        {
            results->add("You must be in a package directory if you want to run this handler without arguments.");
            throw ActionException();
        }

        unsigned short depth = 2;
        std::string leftover;
        std::string path(util::getcwd());

        while (depth > 0)
        {
            std::string::size_type pos = path.rfind('/');
            if (pos != std::string::npos)
            {
                leftover = (leftover.empty() ? path.substr(pos + 1) :
                                               path.substr(pos + 1)+"/"+leftover);
                path.erase(pos);
            }
            --depth;
        }
        
        /* now assign portdir to our path, treating the leftovers as the
         * category or category/package */
        pwd = true;
        dir = path;
        query.push_back(std::make_pair("", leftover));
    }
    else if (options.regex())
    {
        regexp.assign(query.front().second);
        query.clear();

        pkgcache& pkgcache(GlobalPkgCache());
        matches = portage::find_package_regex(regexp, options.overlay(),
                    &search_timer, pkgcache);

        if (matches.empty())
        {
            results->add("Failed to find any packages matching '" + regexp() + "'.");
            throw ActionException();
        }
    }

    std::transform(query.begin(), query.end(),
        std::inserter(matches, matches.end()), util::EmptyFirst());

    std::multimap<std::string, std::string>::iterator m;
    std::multimap<std::string, std::string>::size_type n = 1;
    for (m = matches.begin() ; m != matches.end() ; ++m, ++n)
    {
        std::string package;

        try
        {
            if (pwd)
                package = portage::find_package_in(dir,
                            m->second, &search_timer);
            else if (options.regex() and not m->first.empty())
            {
                dir = m->first;
                package = m->second;
            }
            else
            {
                std::pair<std::string, std::string> p =
                    portage::find_package(m->second, options.overlay(),
                        &search_timer);

                dir = p.first;
                package = p.second;
            }
            if (dir != options.portdir() and not pwd)
                od.insert(dir);

            portage::versions versions(dir + "/" + package);

            /* versions would be empty if the directory exists, but no
             * ebuilds are there - in this case, use real PORTDIR. */
            if (versions.empty())
                versions.assign(options.portdir() + "/" + package);
            /* still empty, so bail */
            if (versions.empty())
                throw portage::NonExistentPkg(package);

            this->size() += versions.size();

            if (not options.quiet())
                results->add("Package", (dir == options.portdir() or pwd) ?
                        package : package+od[dir]);

            if (not options.count())
            {
                portage::versions::iterator v;
                for (v = versions.begin() ; v != versions.end() ; ++v)
                {
                    const portage::version_map& vmap(v->components());

                    std::string s(vmap["PVR"]);
                    std::string::size_type pos = s.rfind("-r0");
                    if (pos != std::string::npos)
                        s.erase(pos);

                    results->add(s);
                }
            }
            if (not options.count() and (n != matches.size()))
                results->add_linebreak();
        }
        catch (const portage::AmbiguousPkg &e)
        {
            results->add(e.name() + " is ambiguous.  Possible matches are:");
            results->add_linebreak();

            opts_type::const_iterator i;
            for (i = e.packages.begin() ; i != e.packages.end() ; ++i)
            {
                if (options.quiet() or not options.color())
                    results->add(*i);
                else
                    results->add(color[green] + (*i) + color[none]);
            }
            
            if (matches.size() == 1 and options.iomethod() == "stream")
                throw ActionException();
        }
        catch (const portage::NonExistentPkg &e)
        {
            results->add(m->second + " doesn't seem to exist.");

            if (matches.size() == 1 and options.iomethod() == "stream")
                throw ActionException();
        }
    }

    PortageSearchActionHandler::operator()(query, results);
}

/* vim: set tw=80 sw=4 fdm=marker et : */
