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
#include <herdstat/portage/package_which.hh>

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
WhichActionHandler::do_results(Query& query,
                               QueryResults * const results)
{
    BacktraceContext c("WhichActionHandler::do_results()");

    this->size() = 0;

    if (not options.regex())
    {
        for (Query::iterator q = query.begin() ; q != query.end() ; ++q)
        {
            try
            {
                const std::vector<portage::Package>& res(find().results());
                find()(q->second, &search_timer);
                if (is_ambiguous(res))
                    throw portage::AmbiguousPkg(res.begin(), res.end());

                matches.insert(matches.end(), res.begin(), res.end());
                find().clear_results();

                if (not options.overlay())
                {
                    remove_overlay_packages();

                    /* might be empty if the pkg only exists in an overlay */
                    if (matches.empty())
                        throw portage::NonExistentPkg(q->second);
                }
            }
            catch (const portage::AmbiguousPkg& e)
            {
                results->add(e.name() + " is ambiguous.  Possible matches are:");
                results->add_linebreak();

                std::for_each(e.packages.begin(), e.packages.end(),
                    std::bind2nd(ColorAmbiguousPkg(), results));
            
                if (query.size() == 1 and options.iomethod() == "stream")
                    throw ActionException();
            }
            catch (const portage::NonExistentPkg& e)
            {
                results->add(e.what());

                if (query.size() == 1 and options.iomethod() == "stream")
                    throw ActionException();
            }
        }
    }

    portage::PackageWhich which;
    const std::vector<std::string>& which_results(which(matches));
    std::copy(which_results.begin(), which_results.end(),
            std::back_inserter(*results));

    this->size() = which_results.size();
}

/* vim: set tw=80 sw=4 fdm=marker et : */
