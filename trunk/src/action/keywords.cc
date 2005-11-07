/*
 * herdstat -- src/action/keywords.cc
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
#include <herdstat/portage/keywords.hh>

#include "common.hh"
#include "pkgcache.hh"
#include "overlaydisplay.hh"
#include "action/keywords.hh"

using namespace herdstat;
using namespace gui;

bool
KeywordsActionHandler::allow_empty_query() const
{
    return true;
}

const char * const
KeywordsActionHandler::id() const
{
    return "keywords";
}

const char * const
KeywordsActionHandler::desc() const
{
    return "Get keywords for the given packages.";
}

const char * const
KeywordsActionHandler::usage() const
{
    return "keywords <package(s)>";
}

Tab *
KeywordsActionHandler::createTab(WidgetFactory *widgetFactory)
{
    Tab *tab = widgetFactory->createTab();
    tab->set_title(this->id());

    return tab;
}

struct GetKeywords
{
    std::pair<std::string, std::string>
    operator()(const portage::version_string& v) const
    {
        std::pair<std::string, std::string> result;
        
        const std::string& pvr((v.components())["PVR"]);
        result.first.assign(pvr.substr(0, pvr.rfind("-r0")));

        /* get keywords for this version */
        try
        {
            portage::Keywords kw(v.ebuild(), GlobalOptions().color());
            result.second.assign(kw.str());
        }
        catch (const Exception& e)
        {
            result.second.assign("no KEYWORDS variable defined");
        }

        return result;
    }
};

void
KeywordsActionHandler::do_all(Query& query, QueryResults * const results)
{
    results->add("This action does not support the 'all' target.");
    throw ActionException();
}

void
KeywordsActionHandler::do_regex(Query& query, QueryResults * const results)
{
    regexp.assign(query.front().second);
    query.clear();

    matches = portage::find_package_regex(regexp, options.overlay(),
                    &search_timer, GlobalPkgCache());

    if (matches.empty())
    {
        results->add("Failed to find any packages matching '" + regexp() + "'.");
        throw ActionException();
    }
}

void
KeywordsActionHandler::do_results(Query& query, QueryResults * const results)
{
    OverlayDisplay od(results);
    std::string dir;
    bool pwd = false;

    this->size() = 0;

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
        query.add(leftover);
    }

    for (Query::iterator q = query.begin() ; q != query.end() ; ++q)
        matches.insert(std::make_pair("", q->second));

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
                results->add("Package",
                        (dir == options.portdir() or pwd) ?
                            package : package+od[dir]);

            /* insert version/keywords pair for each version into results */
            if (not options.count())
                std::transform(versions.begin(), versions.end(),
                    std::back_inserter(*results), GetKeywords());

            if (not options.count() and (n != matches.size()))
                results->add_linebreak();
        }
        catch (const portage::AmbiguousPkg &e)
        {
            results->add(e.name() + " is ambiguous.  Possible matches are:");
            results->add_linebreak();

            std::for_each(e.packages.begin(), e.packages.end(),
                std::bind2nd(ColorAmbiguousPkg(), results));
            
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
}

/* vim: set tw=80 sw=4 fdm=marker et : */
