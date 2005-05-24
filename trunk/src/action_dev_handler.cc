/*
 * herdstat -- src/action_dev_handler.cc
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

#include <memory>

#include "common.hh"
#include "herds_xml.hh"
#include "action_dev_handler.hh"

/*
 * Display data for the specified developer.
 */

void
action_dev_handler_T::display(const util::string &dev)
{
    herds_xml_T::devinfo_T info = herds_xml.get_dev_info(dev);

    /* was the dev in any of the herds? */
    if (info.herds.empty())
        throw dev_E();

    if (not quiet)
    {
        if (info.name.empty())
            output("Developer", dev);
        else
            output("Developer", info.name + " (" + dev + ")");
        
        output("Email", dev + "@gentoo.org");
    }

    if (verbose and not quiet)
    {
        output(util::sprintf("Herds(%d)", info.herds.size()), "");

        opts_type::iterator i;
        herds_xml_T::herds_type::size_type nh = 1;
        for (i = info.herds.begin() ; i != info.herds.end() ; ++i, ++nh)
        {
            /* display herd */
            if (optget("color", bool))
                output("", color[blue] + (*i) + color[none]);
            else
                output("", *i);
                        
            /* display herd info */
            if (not herds_xml[*i]->mail.empty())
                output("", herds_xml[*i]->mail);
            if (not herds_xml[*i]->desc.empty())
                output("", herds_xml[*i]->desc);

            if (nh != info.herds.size())
                output.endl();
        }
    }
    else if (not count)
        output(util::sprintf("Herds(%d)", info.herds.size()), info.herds);

    size += info.herds.size();
}

/*
 * Given a list of developers, display all herds that
 * each developer belongs to.
 */

int
action_dev_handler_T::operator() (opts_type &devs)
{
    herds_xml_T::const_iterator h;

    herds_xml.fetch();
    herds_xml.parse();

    /* set format attributes */
    output.set_maxlabel(all ? 16 : 12);
    output.set_maxdata(maxcol - output.maxlabel());
    output.set_attrs();

    /* all target? */
    if (all)
    {
        /* treat ALL developers in herds.xml as a single herd */
        herds_xml_T::herd_type all_devs;
        for (h = herds_xml.begin() ; h != herds_xml.end() ; ++h)
        {
            /* for each developer in the herd... */
            herds_xml_T::herd_type::iterator d;
            for (d = h->second->begin() ; d != h->second->end() ; ++d)
            {
                /* if the developer is not already in our list, add it */
                if (all_devs.find(d->first) == all_devs.end())
                    all_devs[d->first] = d->second;
            }
        }

        all_devs.display(*stream);
        size = all_devs.size();
        flush();
        return EXIT_SUCCESS;
    }
    else if (regex and devs.size() > 1)
    {
        std::cerr << "You may only specify one regular expression."
            << std::endl;
        return EXIT_FAILURE;
    }
    else if (regex)
    {
        util::regex_T::string_type re(devs.front());
        devs.clear();

        regexp.assign(re, eregex ? REG_EXTENDED|REG_ICASE : REG_ICASE);

        /* loop through herds searching for devs who's username
         * matches the regular expression */
        for (h = herds_xml.begin() ; h != herds_xml.end() ; ++h)
        {
            herds_xml_T::herd_type::iterator d;
            herds_xml_T::string_type herd = h->first;

            for (d = herds_xml[herd]->begin() ; 
                 d != herds_xml[herd]->end() ; ++d)
            {
                if (regexp == d->first)
                    devs.push_back(util::get_user_from_email(d->first));
            }
        }

        if (devs.empty())
        {
            std::cerr << "Failed to find any developers matching '" << re
                << "'." << std::endl;
            return EXIT_FAILURE;
        }

        std::sort(devs.begin(), devs.end());
        devs.erase(std::unique(devs.begin(), devs.end()), devs.end());
    }

    /* for each specified dev... */
    opts_type::iterator dev;
    opts_type::size_type n = 1;
    for (dev = devs.begin() ; dev != devs.end() ;  ++dev, ++n)
    {
        try
        {
            display(*dev);
        }
        catch (const dev_E)
        {
            std::cerr << "Developer '" << *dev << "' doesn't seem to "
                << "belong to any herds." << std::endl;

            if (devs.size() == 1)
                return EXIT_FAILURE;
            else
                continue;
        }

        /* skip a line if we're not displaying the last one */
        if (not count and n != devs.size())
            output.endl();
    }

    flush();
    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
