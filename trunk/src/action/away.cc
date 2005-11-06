/*
 * herdstat -- src/action/away.cc
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

#include <herdstat/util/algorithm.hh>
#include <herdstat/util/functional.hh>
#include <herdstat/portage/functional.hh>

#include "common.hh"
#include "action/away.hh"

using namespace herdstat;
using namespace gui;

const char * const
AwayActionHandler::id() const
{
    return "away";
}

const char * const
AwayActionHandler::desc() const
{
    return "Display away developers and their away messages.";
}

const char * const
AwayActionHandler::usage() const
{
    return "away <developer(s)>";
}

Tab *
AwayActionHandler::createTab(WidgetFactory *widgetFactory)
{
    Tab *tab = widgetFactory->createTab();
    tab->set_title(this->id());

    return tab;
}

void
AwayActionHandler::operator()(Query& query,
                              QueryResults * const results)
{
    const portage::Developers& devs(GlobalDevawayXML().devs());
    portage::Developers::const_iterator d;

    if (query.all())
        transform_to_query(devs.begin(), devs.end(),
            query, portage::User());
    else if (options.regex())
    {
        regexp.assign(query.front().second);
        query.clear();

        transform_to_query_if(devs.begin(), devs.end(), query,
            std::bind1st(portage::UserRegexMatch<portage::Developer>(),
                regexp), portage::User());

        if (query.empty())
        {
            results->add("Failed to find any developers matching '" +
                         regexp() + "'.");
            throw ActionException();
        }
    }

    for (Query::iterator q = query.begin() ; q != query.end() ; ++q)
    {
        try
        {
            if ((d = devs.find(q->second)) == devs.end())
                throw ActionException();

            this->size()++;

            if (options.count())
                continue;

            if (options.quiet())
                results->add(
                    d->user() + " - " + util::tidy_whitespace(d->awaymsg()));
            else
            {
                /* copy what we have and fill it with info in herds.xml */
                portage::Developer dev(*d);
                GlobalHerdsXML().fill_developer(dev);

                if (dev.name().empty())
                    results->add("Developer", dev.user());
                else
                    results->add("Developer",
                         dev.name() + " (" + dev.user() + ")");

                results->add("Email", dev.email());
                results->add("Away Message",
                     util::tidy_whitespace(dev.awaymsg()));
            }
        }
        catch (const ActionException)
        {
            this->error() = true;
            results->add(util::sprintf(
                "Developer '%s' either doesn't exist or is not currently away.",
                q->second.c_str()));

            if (query.size() == 1 and options.iomethod() == "stream")
                throw;
        }

        if (not options.quiet() and ((q+1) != query.end()))
            results->add_linebreak();
    }

    ActionHandler::operator()(query, results);
}

/* vim: set tw=80 sw=4 fdm=marker et : */
