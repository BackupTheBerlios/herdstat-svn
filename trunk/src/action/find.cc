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
FindActionHandler::do_results(Query& query, QueryResults * const results)
{
    BacktraceContext c("FindActionHandler::do_results()");
        
    this->size() = 0;

    if (not options.regex())
    {
        const std::vector<portage::Package>& find_results(find().results());
        for (Query::iterator q = query.begin() ; q != query.end() ; ++q)
        {
            try
            {
                find()(q->second, &search_timer);
            }
            catch (const portage::NonExistentPkg& e)
            {
                results->add(e.what());

                if (query.size() == 1 and options.iomethod() == "stream")
                    throw ActionException();
            }

            matches.insert(matches.end(),
                find_results.begin(), find_results.end());
            find().clear_results();
        }
    }

    /* if not in overlay mode, remove those packages
     * that were found in an overlay. */
    if (not options.overlay())
        remove_overlay_packages();

    if (matches.size() > 1)
    {
//        std::sort(matches.begin(), matches.end());
        matches.erase(std::unique(matches.begin(), matches.end(),
                portage::FullPkgNameEqual()), matches.end());
    }

    this->size() = matches.size();

    if (options.meta())
    {
        const bool re = options.regex();
        const bool ere = options.eregex();

        /* disable stuff we've already handled */
        options.set_regex(false);
        options.set_eregex(false);

        Query q;
        std::transform(matches.begin(), matches.end(),
            std::back_inserter(q), portage::FullPkgName());

        MetaActionHandler mhandler;
        mhandler(q, results);

        options.set_regex(re);
        options.set_eregex(ere);
    }
    else if (not options.count())
        std::transform(matches.begin(), matches.end(),
            std::back_inserter(*results), portage::FullPkgName());
}

/* vim: set tw=80 sw=4 fdm=marker et : */
