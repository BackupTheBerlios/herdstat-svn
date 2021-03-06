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

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "herds.hh"
#include "xmlparser.hh"
#include "herds_xml_handler.hh"
#include "util.hh"
#include "options.hh"
#include "exceptions.hh"
#include "formatter.hh"
#include "action_dev_handler.hh"

/*
 * Given a list of developers, display all herds that
 * each developer belongs to.
 */

int
action_dev_handler_T::operator() (herds_T &herds_xml,
                                  std::vector<std::string> &devs)
{
    util::color_map_T color;
    std::ostream *stream = optget("outstream", std::ostream *);
    
    /* set format attributes */
    formatter_T output;
    output.set_maxlabel(16);
    output.set_maxdata(optget("maxcol", size_t) - output.maxlabel());
    output.set_attrs();

    herds_T::iterator h;

    /* all target? */
    if (devs[0] == "all")
    {
        /* treat ALL developers in herds.xml as a single herd */
        herd_T all_devs;
        for (h = herds_xml.begin() ; h != herds_xml.end() ; ++h)
        {
            herd_T::iterator d;
            for (d = h->second->begin() ; d != h->second->end() ; ++d)
            {
                /* if the developer is not already in our list, add it */
                if (all_devs.find(d->first) == all_devs.end())
                    all_devs[d->first] = d->second;
            }
        }
        all_devs.display(*stream);
    }
    else
    {
        /* for each specified dev... */
        std::vector<std::string>::iterator dev;
        std::vector<std::string>::size_type n = 0;
        for (dev = devs.begin() ; dev != devs.end() ; ++dev)
        {
            std::vector<std::string> herds;

            /* for each herd in herds.xml... */
            herds_T::size_type nherd = 0;
            for (h = herds_xml.begin() ; h != herds_xml.end() ; ++h)
            {
                std::string herd = h->first;

                /* is the dev in the current herd? */
                herd_T::iterator d = herds_xml[herd]->find(*dev + "@gentoo.org");
                if (d != herds_xml[herd]->end())
                    herds.push_back(herd);
                else
                {
                    /* there have been cases where the dev put just their
                     * nick instead of their full email address             */
                    d = herds_xml[herd]->find(*dev);
                    if (d != herds_xml[herd]->end())
                        herds.push_back(herd);
                }
            }

            /* was the dev in any of the herds? */
            if (herds.empty())
            {
                std::cerr << color[red] << "'" << *dev << "' doesn't seem to "
                    << "belong to any herds." << color[none] << std::endl;

                if (devs.size() == 1)
                    throw dev_E();
            }
            else
            {
                if (not optget("quiet", bool))
                    output.append("Developer", *dev);

                if (optget("verbose", bool))
                {
                    output.append(util::sprintf("Herds(%d)", herds.size()), "");

                    std::vector<std::string>::iterator i;
                    for (i = herds.begin() ; i != herds.end() ; ++i)
                    {
                        /* display herd */
                        output.append("", color[blue] + (*i) + color[none]);
                        
                        /* display herd info */
                        if (not herds_xml[*i]->mail.empty())
                            output.append("", herds_xml[*i]->mail);
                        if (not herds_xml[*i]->desc.empty())
                            output.append("", herds_xml[*i]->desc);

                        if (++nherd != herds.size())
                            output.endl();
                    }
                }
                else
                    output.append(util::sprintf("Herds(%d)", herds.size()), herds);
            }

            /* skip a line if we're not displaying the last one */
            if (++n != devs.size())
                output.endl();
        }
    }

    output.flush(*stream);

    if (optget("timer", bool))
        *stream << std::endl;

    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
