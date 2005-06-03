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

#include "common.hh"
#include "metacache.hh"
#include "action_stats_handler.hh"

void
action_stats_handler_T::show_meta_stats()
{
//    output("Total package metadata.xml's:",
//        util::sprintf("%d", metacache.size()));
}

/*
 * Display statistics summary.
 */

int
action_stats_handler_T::operator() (opts_type &null)
{
    optset("quiet", bool, false);

    herds_xml.fetch();
    herds_xml.parse();
    devaway.fetch();
    devaway.parse();

//    if (metastats)
//        metacache.load();

    /* set format attributes */
    output.set_maxlabel(35);
    output.set_maxdata(maxcol - output.maxlabel());
    output.set_devaway(devaway.keys());
    output.set_attrs();

    herds_xml_T::const_iterator h;
    herds_xml_T::herd_type::iterator d;

    float nherds = 0, ndevs = 0;
    std::vector<util::string> most_herds, least_herds, most_devs, least_devs;
    unsigned short biggest_dev = 0, smallest_dev = 1;
    herds_xml_T::herd_type::size_type biggest_herd = 0, smallest_herd = 1;
    std::map<util::string, unsigned short> herds_per_dev;

    /* for each herd in herds.xml... */
    for (h = herds_xml.begin() ; h != herds_xml.end() ; ++h)
    {
        ndevs += h->second->size();

        /* add one to the number of herds the current dev is in */
        for (d = h->second->begin() ; d != h->second->end() ; ++d)
            ++herds_per_dev[d->first];

        /* is the size of this herd bigger than the previous biggest herd */
        if (h->second->size() > biggest_herd)
            biggest_herd = h->second->size();

        if (h->second->size() <= smallest_herd)
            smallest_herd = h->second->size();
    }

    /* for each developer in herds.xml */
    std::map<util::string, unsigned short>::iterator i;
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
    for (h = herds_xml.begin() ; h != herds_xml.end() ; ++h)
    {
        if (h->second->size() == biggest_herd)
            most_devs.push_back(h->first);
        else if (h->second->size() == smallest_herd)
            least_devs.push_back(h->first);
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
    output("Total herds", util::sprintf("%d", herds_xml.size()));
    output("Total devs", util::sprintf("%d", herds_per_dev.size()));
    output("Avg devs/herd", util::sprintf("%.2f",
        ndevs / herds_xml.size()));
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

//    if (metastats)
//        show_meta_stats();

    count = false;
    flush();
    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
