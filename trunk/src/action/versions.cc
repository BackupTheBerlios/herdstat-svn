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
#include <herdstat/portage/package.hh>
#include <herdstat/portage/version.hh>

#include "common.hh"
#include "overlay_display.hh"
#include "action/versions.hh"

using namespace herdstat;
using namespace gui;

bool
VersionsActionHandler::allow_pwd_query() const
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
VersionsActionHandler::do_results(Query& query,
                                  QueryResults * const results)
{
    BacktraceContext c("VersionsActionHandler::do_results()");

    OverlayDisplay od(results);

    this->size() = 0;

    if (not options.regex() and not pwd_mode())
    {
        for (Query::iterator q = query.begin() ; q != query.end() ; ++q)
        {
            try
            {
                const std::vector<portage::Package>& res(find().results());
                find()(q->second, spinner);
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

                std::for_each(e.packages().begin(), e.packages().end(),
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

    std::vector<portage::Package>::iterator m;
    for (m = matches.begin() ; m != matches.end() ; ++m)
    {
        const portage::KeywordsMap& versions(m->keywords());

        this->size() += versions.size();

        if (m->portdir() != options.portdir() and not pwd_mode())
            od.insert(m->portdir());

        if (not options.quiet())
        {
            results->add("Package",
                    (m->portdir() == options.portdir() or pwd_mode()) ?
                        m->full() : m->full()+od[m->portdir()]);
            std::transform(versions.begin(), versions.end(),
                    std::back_inserter(*results),
                    util::compose_f_gx(
                        std::mem_fun_ref(&portage::VersionString::str),
                        util::First<portage::KeywordsMap::value_type>()));
        }
        else if (not options.count())
            results->transform(versions.begin(), versions.end(),
                util::compose_f_gx(
                    std::mem_fun_ref(&portage::VersionString::str),
                    util::First<portage::KeywordsMap::value_type>()));
            
        if (not options.count() and ((m+1) != matches.end()))
            results->add_linebreak();
    }
}

/* vim: set tw=80 sw=4 fdm=marker et : */
