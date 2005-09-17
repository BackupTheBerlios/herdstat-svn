/*
 * herdstat -- src/action_stats_handler.cc
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

#include <vector>
#include <algorithm>
#include <herdstat/util/string.hh>
#include <herdstat/portage/herd.hh>

#include "common.hh"
#include "metacache.hh"
#include "action_stats_handler.hh"

using namespace portage;

action_stats_handler_T::~action_stats_handler_T()
{
}

/*
 * Display statistics summary.
 */

int
action_stats_handler_T::operator() (opts_type &null)
{
    optset("quiet", bool, false);

    fetch_herdsxml();
    herdsxml.parse(herdsxml_path);
    const Herds& herds(herdsxml.herds());

    if (use_devaway)
    {
        fetch_devawayxml();
        devaway.parse(devaway_path);
    }

    /* set format attributes */
    output.set_maxlabel(35);
    output.set_maxdata(maxcol - output.maxlabel());
    if (use_devaway)
        output.set_devaway(devaway.keys());
    output.set_attrs();

    Herds::const_iterator h;
    Herd::const_iterator d;

    float nherds = 0, ndevs = 0;
    std::vector<std::string> most_herds, least_herds, most_devs, least_devs;
    unsigned short biggest_dev = 0, smallest_dev = 1;
    Herd::size_type biggest_herd = 0, smallest_herd = 1;
    std::map<std::string, unsigned short> herds_per_dev;

    /* for each herd in herds.xml... */
    for (h = herds.begin() ; h != herds.end() ; ++h)
    {
        ndevs += h->size();

        /* add one to the number of herds the current dev is in */
        for (d = h->begin() ; d != h->end() ; ++d)
            ++herds_per_dev[d->user()];

        /* is the size of this herd bigger than the previous biggest herd */
        if (h->size() > biggest_herd)
            biggest_herd = h->size();

        if (h->size() <= smallest_herd)
            smallest_herd = h->size();
    }

    /* for each developer in herds.xml */
    std::map<std::string, unsigned short>::iterator i;
    for (i = herds_per_dev.begin() ; i != herds_per_dev.end() ; ++i)
    {
        nherds += i->second;

        /* is the no. of herds this dev is in bigger than previous biggest? */
        if (i->second > biggest_dev)
            biggest_dev = i->second;

        if (i->second <= smallest_dev)
            smallest_dev = i->second;
    }

    /* we now have least/most devs, so find all devs with matching numbers */
    for (h = herds.begin() ; h != herds.end() ; ++h)
    {
        if (h->size() == biggest_herd)
            most_devs.push_back(h->name());
        else if (h->size() == smallest_herd)
            least_devs.push_back(h->name());
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
    output("Total herds", util::sprintf("%d", herds.size()));
    output("Total devs", util::sprintf("%d", herds_per_dev.size()));
    output("Avg devs/herd", util::sprintf("%.2f",
        ndevs / herds.size()));
    output("Avg herds/dev", util::sprintf("%.2f",
        nherds / herds_per_dev.size()));
    output(util::sprintf("Herd(s) with most devs(%d)", biggest_herd),
        most_devs);
    output(util::sprintf("Herd(s) with least devs(%d)", smallest_herd),
        least_devs);
    output(util::sprintf("Dev(s) belonging to most herds(%d)", biggest_dev),
        most_herds);
    output(util::sprintf("Dev(s) belonging to least herds(%d)", smallest_dev),
        least_herds);

    count = false;
    flush();
    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
