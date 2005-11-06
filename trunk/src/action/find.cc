/*
 * herdstat -- src/action/find.cc
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

#include <algorithm>
#include <iterator>
#include <herdstat/util/functional.hh>
#include <herdstat/portage/find.hh>
#include <herdstat/portage/exceptions.hh>

#include "common.hh"
#include "pkgcache.hh"
#include "action/meta.hh"
#include "action/find.hh"

using namespace herdstat;
using namespace gui;

const char * const
FindActionHandler::id() const
{
    return "find";
}

const char * const
FindActionHandler::desc() const
{
    return "Find packages matching the given criteria.";
}

const char * const
FindActionHandler::usage() const
{
    return "find <package(s)>";
}

Tab *
FindActionHandler::createTab(WidgetFactory *widgetFactory)
{
    Tab *tab = widgetFactory->createTab();
    tab->set_title(this->id());

    return tab;
}

void
FindActionHandler::operator()(Query& query,
                              QueryResults * const results)
{
    if (query.all())
    {
        results->add("'find' handler does not support the all target.");
        throw ActionException();
    }

    pkgcache& pkgcache(GlobalPkgCache());

    if (options.regex())
    {
        regexp.assign(query.front().second);

        matches = portage::find_package_regex(regexp, options.overlay(),
                        &search_timer, pkgcache);

        if (matches.empty())
        {
            results->add("Failed to find any packages matching '" + regexp() + "'.");
            throw ActionException();
        }
    }
    else
        std::transform(query.begin(), query.end(),
            std::inserter(matches, matches.end()), util::EmptyFirst());

    std::vector<std::string> res;
    std::multimap<std::string, std::string>::iterator m;
    for (m = matches.begin() ; m != matches.end() ; ++m)
    {
        std::pair<std::string, std::string> p;

        try
        {
            if (options.regex())
                p = *m;
            else
                p = portage::find_package(m->second, options.overlay(),
                                &search_timer, pkgcache);
        }
        catch (const portage::AmbiguousPkg& e)
        {
            /* ambiguous still matches */
            res.insert(res.end(), e.packages.begin(), e.packages.end());
            continue;
        }
        catch (const portage::NonExistentPkg& e)
        {
            results->add(m->second + " doesn't seem to exist.");

            if (matches.size() == 1 and options.iomethod() == "stream")
                throw ActionException();

            continue;
        }

        res.push_back(p.second);
    }

    if (res.size() > 1)
    {
        std::sort(res.begin(), res.end());
        res.erase(std::unique(res.begin(), res.end()), res.end());
    }

    if (options.meta())
    {
        const bool re = options.regex();
        const bool ere = options.eregex();

        /* disable stuff we've already handled */
        options.set_regex(false);
        options.set_eregex(false);

        Query q;
        transform_to_query(res.begin(), res.end(), q);

        MetaActionHandler mhandler;
        mhandler(q, results);

        options.set_regex(re);
        options.set_eregex(ere);
    }
    else if (not options.count())
        copy_to_results(res.begin(), res.end(), *results);

    this->size() = res.size();
    PortageSearchActionHandler::operator()(query, results);
}

/* vim: set tw=80 sw=4 fdm=marker et : */
