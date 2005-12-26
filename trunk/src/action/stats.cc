/*
 * herdstat -- src/action/stats.cc
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
#include "action/stats.hh"

using namespace herdstat;
using namespace gui;

bool
StatsActionHandler::allow_empty_query() const
{
    return true;
}

const char * const
StatsActionHandler::id() const
{
    return "stats";
}

const char * const
StatsActionHandler::desc() const
{
    return "Show herds.xml statistics.";
}

Tab *
StatsActionHandler::createTab(WidgetFactory *widgetFactory)
{
    Tab *tab = widgetFactory->createTab();
    tab->set_title(this->id());

    return tab;
}

void
StatsActionHandler::generate_completions(std::vector<std::string> *) const
{
}

void
StatsActionHandler::do_regex(Query& query LIBHERDSTAT_UNUSED,
                             QueryResults * const results)
{
    results->add("This action does not support regular expressions.");
    throw ActionException();
}

void
StatsActionHandler::do_results(Query& query LIBHERDSTAT_UNUSED,
                               QueryResults * const results)
{
    BacktraceContext c("StatsActionHandler::do_results()");

    const bool quiet_save(options.quiet());
    options.set_quiet(false);

    const portage::Herds& herds(GlobalHerdsXML().herds());
    portage::Herds::const_iterator h;
    portage::Herd::const_iterator d;

    float nherds = 0, ndevs = 0;
    std::vector<std::string> most_herds, least_herds, most_devs, least_devs;
    portage::Herd::size_type biggest_herd = 0, smallest_herd = 1;
    unsigned short biggest_dev = 0, smallest_dev = 1;
    std::map<std::string, unsigned short> herds_per_dev;

    for (h = herds.begin() ; h != herds.end() ; ++h)
    {
        ndevs += h->size();
        for (d = h->begin() ; d != h->end() ; ++d)
            ++herds_per_dev[d->user()];
        
        if (h->size() > biggest_herd) biggest_herd = h->size();
        if (h->size() <= smallest_herd) smallest_herd = h->size();
    }

    std::map<std::string, unsigned short>::iterator i;
    for (i = herds_per_dev.begin() ; i != herds_per_dev.end() ; ++i)
    {
        nherds += i->second;
        if (i->second > biggest_dev) biggest_dev = i->second;
        if (i->second <= smallest_dev) smallest_dev = i->second;
    }

        /* we now have least/most devs, so find all devs with matching numbers */
    for (h = herds.begin() ; h != herds.end() ; ++h)
    {
        if (h->size() == biggest_herd)          most_devs.push_back(h->name());
        else if (h->size() == smallest_herd)    least_devs.push_back(h->name());
    }
    
    /* we now have least/most herds, so find all herds with matching numbers */
    for (i = herds_per_dev.begin() ; i != herds_per_dev.end() ; ++i)
    {
        if (i->second == biggest_dev)
            most_herds.push_back(util::get_user_from_email(i->first));
        else if (i->second == smallest_dev)
            least_herds.push_back(util::get_user_from_email(i->first));
    }
    
    /* display it all */
    results->add("Total herds", util::sprintf("%d", herds.size()));
    results->add("Total devs", util::sprintf("%d", herds_per_dev.size()));
    results->add("Avg devs/herd", util::sprintf("%.2f",
        ndevs / herds.size()));
    results->add("Avg herds/dev", util::sprintf("%.2f",
        nherds / herds_per_dev.size()));
    results->add(util::sprintf("Herd(s) with most devs(%d)", biggest_herd),
        most_devs);
    results->add(util::sprintf("Herd(s) with least devs(%d)", smallest_herd),
        least_devs);
    results->add(util::sprintf("Dev(s) belonging to most herds(%d)", biggest_dev),
        most_herds);
    results->add(util::sprintf("Dev(s) belonging to least herds(%d)", smallest_dev),
        least_herds);

    options.set_quiet(quiet_save);
}

/* vim: set tw=80 sw=4 fdm=marker et : */
