/*
 * herdstat -- action/herd.cc
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

#include <herdstat/util/string.hh>
#include <herdstat/util/algorithm.hh>
#include <herdstat/util/functional.hh>

#include "common.hh"
#include "action/herd.hh"

using namespace herdstat;
using namespace gui;

void
add_herd(const portage::Herd& herd, QueryResults * const results)
{
    const Options& options(GlobalOptions());

    if (not options.quiet())
    {
        if (not herd.name().empty())
            results->add("Herd", herd.name());
        if (not herd.email().empty())
            results->add("Email", herd.email());
        if (not herd.desc().empty())
            results->add("Description", util::tidy_whitespace(herd.desc()));

        if (options.verbose())
            results->add(util::sprintf("Developers(%d)", herd.size()), "");
    }

    if (options.verbose() and not options.quiet())
    {
        util::ColorMap& color(GlobalColorMap());
        const std::string user(util::current_user());

        portage::Herd::const_iterator i;
        for (i = herd.begin() ; i != herd.end() ; ++i)
        {
            if ((i->user() == user) or not options.color())
                results->add(i->email());
            else
                results->add(color[blue] + i->email() + color[none]);

            if (not i->name().empty())
                results->add(i->name());
            if (not i->role().empty())
                results->add(i->role());
            if (not i->name().empty() or not i->role().empty())
                results->add_linebreak();
        }
    }

    if ((not options.verbose() and not options.quiet()) or
        (not options.verbose() and options.quiet() and not options.count()))
        results->add(util::sprintf("Developers(%d)", herd.size()),
                        herd.begin(), herd.end());
}

static void
add_herds(const portage::Herds& herds, QueryResults * const results)
{
    const Options& options(GlobalOptions());
    util::ColorMap& color(GlobalColorMap());

    if (options.verbose() and not options.quiet())
    {
        results->add(util::sprintf("Herds(%d)", herds.size()), "");

        portage::Herds::size_type n = 1;
        portage::Herds::const_iterator h;
        for (h = herds.begin() ; h != herds.end() ; ++h)
        {
            if (options.color())
                results->add(color[blue] + h->name() + color[none]);
            else
                results->add(h->name());

            if (not h->desc().empty())
                results->add(util::tidy_whitespace(h->desc()));

            if (not options.count() and n != herds.size())
                results->add_linebreak();
        }
    }
    else if (not options.count())
        results->add(util::sprintf("Herds(%d)", herds.size()),
                herds.begin(), herds.end());
}

const char * const
HerdActionHandler::id() const
{
    return "herd";
}

const char * const
HerdActionHandler::desc() const
{
    return "Get information for the given herd(s).";
}

const char * const
HerdActionHandler::usage() const
{
    return "herd <herd(s)>";
}

Tab *
HerdActionHandler::createTab(WidgetFactory *widgetFactory)
{
    Tab *tab = widgetFactory->createTab();
    tab->set_title(this->id());

    /* ... */

    return tab;
}

void
HerdActionHandler::do_all(Query& query, QueryResults * const results)
{
    const portage::Herds& herds(GlobalHerdsXML().herds());
    std::transform(herds.begin(), herds.end(),
        std::back_inserter(query), portage::Name());
}

void
HerdActionHandler::do_regex(Query& query, QueryResults * const results)
{
    const portage::Herds& herds(GlobalHerdsXML().herds());

    regexp.assign(query.front().second);
    query.clear();

    util::transform_if(herds.begin(), herds.end(), std::back_inserter(query),
        std::bind1st(portage::NameRegexMatch<portage::Herd>(), regexp),
        portage::Name());
}

void
HerdActionHandler::do_results(Query& query, QueryResults * const results)
{
    const portage::Herds& herds(GlobalHerdsXML().herds());
    portage::Herds::const_iterator h;

    if (query.all() and options.quiet())
    {
        results->transform(query.begin(), query.end(), util::Second());
        return;
    }

    for (Query::iterator q = query.begin() ; q != query.end() ; ++q)
    {
        if ((h = herds.find(q->second)) == herds.end())
        {
            this->error() = true;
            results->add("Herd '" + q->second + "' doesn't seem to exist.");

            if (options.iomethod() == "stream")
                throw ActionException();

            q = query.erase(q);
        }
        else if (options.count())
            continue;
        else
        {
            if (not options.quiet())
            {
                results->add("Name", h->name());
                results->add("Email", h->email());
            }

            results->add(util::sprintf("Developers(%d)", h->size()),
                         h->begin(), h->end());

            if ((q+1) != query.end())
                results->add_linebreak();
        }
    }
}

/* vim: set tw=80 sw=4 fdm=marker et : */
