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

#include "common.hh"
#include "herds_xml.hh"
#include "formatter.hh"
#include "action_herd_handler.hh"

/*
 * Given a list of herds, display herd and developer
 * information.
 */

int
action_herd_handler_T::operator() (std::vector<util::string> &herds)
{
    std::ostream *stream = optget("outstream", std::ostream *);
    const bool regex = optget("regex", bool);

    /* set format attributes */
    formatter_T output;

    if (optget("all", bool))
        output.set_maxlabel(11);
    else
        output.set_maxlabel(15);

    output.set_maxdata(optget("maxcol", std::size_t) - output.maxlabel());
    output.set_attrs();

    herds_xml_T herds_xml;

    /* was the all target specified? */
    if (optget("all", bool))
    {
        herds_xml.display(*stream);
        
        if (optget("count", bool))
            output("", util::sprintf("%d", herds_xml.size()));

        output.flush(*stream);
        return EXIT_SUCCESS;
    }
    else if (regex and herds.size() > 1)
    {
        std::cerr << "You may only specify one regular expression."
            << std::endl;
        return EXIT_FAILURE;
    }
    else if (regex)
    {
        util::regex_T regexp;

        if (optget("eregex", bool))
            regexp.assign(herds.front(), REG_EXTENDED|REG_ICASE);
        else
            regexp.assign(herds.front(), REG_ICASE);
        
        herds.clear();

        herds_xml_T::iterator h;
        for (h = herds_xml.begin() ; h != herds_xml.end() ; ++h)
        {
            if (regexp == h->first)
                herds.push_back(h->first);
        }
    }

    herd_T::size_type size = 0;

    /* for each specified herd... */
    std::vector<util::string>::iterator herd;
    std::vector<util::string>::size_type n = 1;
    for (herd = herds.begin() ; herd != herds.end() ; ++herd, ++n)
    {
        /* does the herd exist? */
        if (not herds_xml.exists(*herd))
        {
            std::cerr << "Herd '" << *herd << "' doesn't seem to exist."
                << std::endl;

            /* if the user specified more than one herd, then just print
             * the error and keep going; otherwise, we want to exit with
             * an error code */
            if (herds.size() > 1)
            {
                std::cerr << std::endl;
                continue;
            }
            else
                throw herd_E();
        }

        herds_xml[*herd]->display(*stream);
        size += herds_xml[*herd]->size();

        /* only skip a line if we're not displaying the last one */
        if (not optget("count", bool) and n != herds.size())
            output.endl();
    }

    if (optget("count", bool))
        output("", util::sprintf("%d", size));

    output.flush(*stream);
    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
