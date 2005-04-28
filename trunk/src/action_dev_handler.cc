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
#include "formatter.hh"
#include "action_dev_handler.hh"

/*
 * Given a list of developers, display all herds that
 * each developer belongs to.
 */

int
action_dev_handler_T::operator() (std::vector<util::string> &devs)
{
    util::color_map_T color;
    std::ostream *stream = optget("outstream", std::ostream *);
    const bool regex = optget("regex", bool);
    
    /* set format attributes */
    formatter_T output;

    if (devs[0] == "all")
        output.set_maxlabel(16);
    else
        output.set_maxlabel(12);

    output.set_maxdata(optget("maxcol", std::size_t) - output.maxlabel());
    output.set_attrs();

    herds_xml_T herds_xml;
    herds_T::iterator h;

    /* all target? */
    if (devs[0] == "all")
    {
        /*
         * treat ALL developers in herds.xml as a single herd
         */

        herd_T all_devs;
        for (h = herds_xml.begin() ; h != herds_xml.end() ; ++h)
        {
            /* for each developer in the herd... */
            herd_T::iterator d;
            for (d = h->second->begin() ; d != h->second->end() ; ++d)
            {
                /* if the developer is not already in our list, add it */
                if (all_devs.find(d->first) == all_devs.end())
                    all_devs[d->first] = d->second;
            }
        }

        all_devs.display(*stream);

        if (optget("count", bool))
            output("", util::sprintf("%d", all_devs.size()));

        output.flush(*stream);
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
        util::regex_T regexp;
        util::string re(devs.front());
        devs.clear();

        if (optget("eregex", bool))
            regexp.assign(re, REG_EXTENDED|REG_ICASE);
        else
            regexp.assign(re, REG_ICASE);

        for (h = herds_xml.begin() ; h != herds_xml.end() ; ++h)
        {
            herd_T::iterator d;
            util::string herd = h->first;

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
                << std::endl;
            return EXIT_FAILURE;
        }

        std::sort(devs.begin(), devs.end());
        devs.erase(std::unique(devs.begin(), devs.end()), devs.end());
    }

    std::vector<util::string>::size_type size = 0;

    /* for each specified dev... */
    std::vector<util::string>::iterator dev;
    std::vector<util::string>::size_type n = 1;
    for (dev = devs.begin() ; dev != devs.end() ; ++dev, ++n)
    {
        util::string name;
        std::vector<util::string> herds;

        /* for each herd in herds.xml... */
        for (h = herds_xml.begin() ; h != herds_xml.end() ; ++h)
        {
            util::string herd = h->first;
            
            /* is the dev in the current herd? */
            herd_T::iterator d = herds_xml[herd]->find(*dev + "@gentoo.org");
            if (d != herds_xml[herd]->end())
            {
                herds.push_back(herd);
                if (not d->second->name.empty())
                    name = d->second->name;
            }
        }

        size += herds.size();

        /* was the dev in any of the herds? */
        if (herds.empty())
        {
            std::cerr << "Developer '" << *dev << "' doesn't seem to "
                << "belong to any herds." << std::endl;

            if (devs.size() == 1)
                throw dev_E();
        }
        else
        {
            if (not optget("quiet", bool))
            {
                output("Developer",
                    (name.empty() ? *dev : name + " (" + (*dev) + ")"));
                output("Email", *dev + "@gentoo.org");
            }

            if (optget("verbose", bool) and not optget("quiet", bool))
            {
                output(util::sprintf("Herds(%d)", herds.size()), "");

                std::vector<util::string>::iterator i;
                herds_T::size_type nh = 1;
                for (i = herds.begin() ; i != herds.end() ; ++i, ++nh)
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

                    if (nh != herds.size())
                        output.endl();
                }
            }
            else if (not optget("count", bool))
                output(util::sprintf("Herds(%d)", herds.size()), herds);
        }

        /* skip a line if we're not displaying the last one */
        if (not optget("count", bool) and n != devs.size())
            output.endl();
    }

    if (optget("count", bool))
        output("", util::sprintf("%d", size));

    output.flush(*stream);
    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
