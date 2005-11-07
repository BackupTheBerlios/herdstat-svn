/*
 * herdstat -- src/action/which.cc
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

#include <herdstat/util/functional.hh>
#include <herdstat/portage/exceptions.hh>
#include <herdstat/portage/find.hh>

#include "common.hh"
#include "pkgcache.hh"
#include "action/which.hh"

using namespace herdstat;
using namespace gui;

const char * const
WhichActionHandler::id() const
{
    return "which";
}

const char * const
WhichActionHandler::desc() const
{
    return "Like which(1) but for ebuilds.  Gets the path to the latest ebuild for the given package.";
}

const char * const
WhichActionHandler::usage() const
{
    return "which <package(s)>";
}

Tab *
WhichActionHandler::createTab(WidgetFactory *widgetFactory)
{
    Tab *tab = widgetFactory->createTab();
    tab->set_title(this->id());

    return tab;
}

void
WhichActionHandler::do_all(Query& query, QueryResults * const results)
{
    results->add("This action does not support the 'all' target.");
    throw ActionException();
}

void
WhichActionHandler::do_regex(Query& query, QueryResults * const results)
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

    std::vector<std::string> v;
    std::multimap<std::string, std::string>::iterator m;
    for (m = matches.begin() ; m != matches.end() ; ++m)
    {
        if (std::find(v.begin(), v.end(), m->second) == v.end())
            v.push_back(m->second);
        else
            matches.erase(m--);
    }
}

void
WhichActionHandler::do_results(Query& query,
                               QueryResults * const results)
{
    this->size() = 0;

    if (not options.regex())
        for (Query::iterator q = query.begin() ; q != query.end() ; ++q)
            matches.insert(std::make_pair("", q->second));

    std::multimap<std::string, std::string>::iterator m;
    for (m = matches.begin() ; m != matches.end() ; ++m)
    {
        std::string ebuild;
        std::pair<std::string, std::string> p;
        
        try
        {
            if (options.regex())
                p = *m;
            else
                p = portage::find_package(m->second, options.overlay(),
                        &search_timer);
        }
        catch (const portage::AmbiguousPkg &e)
        {
            results->add(e.name() + " is ambiguous. Possible matches are:");
            results->add_linebreak();

            std::for_each(e.packages.begin(), e.packages.end(),
                std::bind2nd(ColorAmbiguousPkg(), results));
            
            if (matches.size() == 1 and options.iomethod() == "stream")
                throw ActionException();

            continue;
        }
        catch (const portage::NonExistentPkg &e)
        {
            results->add(m->second + " doesn't seem to exist.");

            if (matches.size() == 1 and options.iomethod() == "stream")
                throw ActionException();

            continue;
        }
        
        if (options.regex())
            ebuild = portage::ebuild_which(p.second, options.overlay(), NULL,
                GlobalPkgCache());
        else
            ebuild = portage::ebuild_which(p.second, options.overlay(), NULL);

        if (not options.count())
            results->add(ebuild);
    }
    
    this->size() = matches.size();
}

/* vim: set tw=80 sw=4 fdm=marker et : */
