/*
 * herdstat -- src/action_herd_handler.cc
 * $Id$
 * Copyright (c) 2005 Aaron Walker <ka0ttic at gentoo.org>
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
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <cstdlib>
#include <cerrno>
#include <unistd.h>

#include "herds.hh"
#include "formatter.hh"
#include "util.hh"
#include "options.hh"
#include "exceptions.hh"
#include "action_herd_handler.hh"

/*
 * Display data for each developer in the given dev map
 */

void
display_dev_data(fm::formatter_T *out, devs_T &devs)
{
    options_T options;
    util::color_map_T color;

    /* if ECHANGELOG_USER is set try to get the dev email
     * so we can highlight it if it occurs in the output  */
    std::string user;
    std::string::size_type oldlen = 0;
    char *result = getenv("ECHANGELOG_USER");
    if (result)
    {
        user = result;
        std::string::size_type pos = user.find("<");
        if (pos != std::string::npos)
            user = user.substr(pos + 1);

        pos = user.find(">");
        if (pos != std::string::npos)
            user = user.substr(0, pos);
    }

    /* sort devs */
    std::vector<std::string> sorted_devs;
    {
        devs_T::iterator i;
        for (i = devs.begin() ; i != devs.end() ; ++i)
        {
            std::string dev = i->first;
            if (not options.verbose())
            {
                /* strip domain names from email addr */
                std::string::size_type pos = dev.find('@');
                if (pos != std::string::npos)
                {
                    /* is the current user this user? */
                    if (dev == user and not options.quiet())
                    {
                        /* if so, highlight it ; we need to save maxctotal
                         * in order to modify it to compensate for color length
                         */
                        oldlen = out->attr.maxctotal;
                        dev = color[yellow] + dev.substr(0, pos) + color[none];
                    }
                    else
                        dev = dev.substr(0, pos);
                }
            }
            sorted_devs.push_back(dev);
        }
    }
    std::sort(sorted_devs.begin(), sorted_devs.end());

    if (options.verbose())
    {
        out->append(util::sprintf("Developers(%d)", devs.size()), "");

        std::vector<std::string>::iterator i;
        for (i = sorted_devs.begin() ; i != sorted_devs.end() ; ++i)
        {
            /* display full email */
            if (*i == user)
                out->append("", color[yellow] + (*i) + color[none]);
            else
                out->append("", color[blue] + (*i) + color[none]);

            /* display developer attributes (name, role, etc) */
            dev_attrs_T::iterator x;
            for (x = devs[*i]->begin() ; x != devs[*i]->end() ; ++x)
                out->append("", *x);

            out->endl();
        }
    }
    else
    {
        if (options.quiet())
        {
            std::vector<std::string>::iterator i;
            for (i = sorted_devs.begin() ; i != sorted_devs.end() ; ++i)
                std::cout << *i << std::endl;
        }
        else
        {
            /* if the current user is in the herd, we highlighted the nick
             * so adjust maxctotal appropriately                            */
            if (oldlen != 0)
                out->attr.maxctotal = oldlen + color[yellow].length() +
                    color[none].length();

            out->append(util::sprintf("Developers(%d)", devs.size()), sorted_devs);

            /* ...and restore it */
            if (oldlen != 0)
                out->attr.maxctotal = oldlen;
        }
    }
}

int
action_herd_handler_T::operator() (HerdsXMLHandler_T *handler,
                                   std::vector<std::string> &herds)
{
    options_T options;
    util::color_map_T color;

    try
    {
        /* set format attributes */
        fm::formatter_T output;
        output.set_maxlabel(15);
        output.set_maxdata(options.maxcol() - output.maxlabel());
        output.set_colors(true);
        output.set_labelcolor(color[green]);
        output.set_attrs();

        /* was the all target specified? */
        if (herds[0] == "all")
        {
            std::vector<std::string> hvec;
            float ndev = 0;

            if (options.verbose())
                output.append(util::sprintf("Herds(%d)", handler->herds.size()), "");

            herds_T::iterator h;
            herds_T::size_type n = 0;
            for (h = handler->herds.begin() ; h != handler->herds.end() ; ++h)
                ndev += h->second->size();

            if (options.verbose())
                output.append("Avg devs/herd", util::sprintf("%.2f",
                    ndev / handler->herds.size()));

            /* for each herd in herds.xml... */
            for (h = handler->herds.begin() ; h != handler->herds.end() ; ++h)
            {
                if (options.verbose())
                {
                    output.append("", color[blue] + h->first + color[none]);

                    if (not handler->descs[h->first].empty())
                        output.append("", handler->descs[h->first]);

                    if (++n != handler->herds.size())
                        output.endl();
                }
                else
                    hvec.push_back(h->first);
            }

            if (not options.verbose())
            {
                output.append(util::sprintf("Herds(%d)", handler->herds.size()),
                    hvec);
                output.append("Avg devs/herd", util::sprintf("%.2f",
                    ndev / handler->herds.size()));
            }
        }

        /* nope, so only display stats for specified herds */
        else
        {
            /* for each specified herd... */
            std::vector<std::string>::iterator herd;
            std::vector<std::string>::size_type n = 0;
            for (herd = herds.begin() ; herd != herds.end() ; ++herd)
            {
                /* does the herd exist? */
                if (not handler->exists(*herd))
                {
                    std::cerr << color[red] << "Herd '" << *herd
                        << "' doesn't seem to exist." << color[none] << std::endl;

                    /* if the user specified more than one herd, then just print
                     * the error and keep going; otherwise, we want to exit with
                     * an error code
                     */
                    if (herds.size() > 1)
                    {
                        std::cerr << std::endl;
                        continue;
                    }
                    else
                        throw herd_E();
                }

                /* ...display it's data */
                if (not options.quiet())
                {
                    output.append("Herd", *herd);
                    output.append("Description", handler->descs[*herd]);
                }

                display_dev_data(&output, *(handler->herds[*herd]));

                /* only skip a line if we're not displaying the last one */
                if (++n != herds.size())
                    output.endl();
            }
        }
        
        if (not options.quiet())
            output.flush(std::cout);
    }
    catch(...)
    {
        throw;
    }

    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
