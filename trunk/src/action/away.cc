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
AwayActionHandler::operator()(const Query& query,
                              QueryResults * const results)
{
    portage::devaway_xml& devaway_xml(GlobalDevawayXML());

//    GlobalFormatter().attrs().set_quiet(options.quiet(), " ");

    const portage::Developers& devs(devaway_xml.devs());
    portage::Developers::const_iterator d;

    if (query.all())
    {

    }
    else if (options.regex())
    {

    }

    for (Query::const_iterator q = query.begin() ; q != query.end() ; ++q)
    {
        try
        {
            if ((d = devs.find(q->second)) == devs.end())
                throw ActionException();

            this->size()++;

            if (not options.count())
            {
                portage::Developer dev(*d);
                GlobalHerdsXML().fill_developer(dev);

                if (dev.name().empty())
                    results->add("Developer", dev.user());
                else
                    results->add("Developer",
                                 dev.name() + " (" + dev.user() + ")");

                results->add("Email", dev.email());
                results->add("Away message",
                             util::tidy_whitespace(dev.awaymsg()));
            }
        }
        catch (const ActionException)
        {
            this->error() = true;
            results->add(util::sprintf(
                "Developer '%s' either doesn't exist or is not currently away.",
                q->second.c_str()));
        }
    }

    ActionHandler::operator()(query, results);
}

/* vim: set tw=80 sw=4 fdm=marker et : */
