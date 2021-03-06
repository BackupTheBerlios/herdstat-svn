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
#include <herdstat/portage/util.hh>
#include <herdstat/portage/package.hh>
#include <herdstat/portage/version.hh>
#include <herdstat/portage/keywords.hh>

#include "common.hh"
#include "overlay_display.hh"
#include "action/keywords.hh"

using namespace herdstat;
using namespace gui;

bool
KeywordsActionHandler::allow_pwd_query() const
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

struct GetVerKeywordsPair
{
    std::pair<std::string, std::string>
    operator()(const std::pair<portage::VersionString, portage::Keywords>& p) const
    {
        if (not GlobalOptions().color())
            return std::make_pair(p.first.str(), util::strip_colors(p.second.str()));
        return std::make_pair(p.first.str(), p.second.str());
    }
};

void
KeywordsActionHandler::do_results(Query& query, QueryResults * const results)
{
    BacktraceContext c("KeywordsActionHandler::do_results()");

    OverlayDisplay od(results);

    this->size() = 0;

    if (not options.regex() and not pwd_mode())
    {
        for (Query::iterator q = query.begin() ; q != query.end() ; ++q)
        {
            try
            {
                const std::vector<portage::Package>& res(find().results());
                find()(q->second, spinner());
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
        if (portage::is_category(m->path()))
        {
            if (options.regex())
                continue;

            results->add("Requesting keywords for a category doesn't make much sense.");
            continue;
        }

        const portage::KeywordsMap& keywords(m->keywords());

        this->size() += keywords.size();

        if (m->in_overlay() and not pwd_mode())
            od.insert(m->portdir());

        if (not options.quiet())
            results->add("Package",
                    (m->portdir() == options.portdir() or pwd_mode()) ?
                        m->full() : m->full()+od[m->portdir()]);

        if (not options.count())
        {
            std::transform(keywords.begin(), keywords.end(),
                std::back_inserter(*results), GetVerKeywordsPair());

            if ((m+1) != matches.end())
                results->add_linebreak();
        }
    }
}

/* vim: set tw=80 sw=4 fdm=marker et : */
