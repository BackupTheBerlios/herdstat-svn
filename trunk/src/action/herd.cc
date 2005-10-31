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

#include "common.hh"
#include "action/herd.hh"

using namespace herdstat;
using namespace herdstat::portage;
using namespace gui;

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
HerdActionHandler::operator()(const Query& query,
                              QueryResults * const results)
{
    /* search for items in query and insert results */
    const portage::Herds& herds(GlobalHerdsXML().herds());
    portage::Herds::const_iterator h;

    for (Query::const_iterator q = query.begin() ; q != query.end() ; ++q)
    {
        try
        {
            if ((h = herds.find(q->second)) == herds.end())
                throw ActionException();

            if (not options.quiet())
            {
                results->add("Name", h->name());
                results->add("Email", h->email());
            }
            
            if (not options.count())
            {
                std::vector<std::string> devs(h->begin(), h->end());
                results->add(util::sprintf("Developers(%d)", h->size()), devs);
            }

            if ((q+1) != query.end())
                results->add_linebreak();

            this->size() += h->size();
        }
        catch (const ActionException)
        {
            this->error() = true;
            results->add(util::sprintf("Herd '%s' doesn't seem to exist.",
                q->second.c_str()));
        }
    }

    ActionHandler::operator()(query, results);
}

/* vim: set tw=80 sw=4 fdm=marker et : */
