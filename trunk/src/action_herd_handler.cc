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

#include <fstream>
#include <memory>
#include <algorithm>

#include "common.hh"
#include "herds_xml.hh"
#include "action_herd_handler.hh"

/*
 * Given a list of herds, display herd and developer
 * information.
 */

int
action_herd_handler_T::operator() (opts_type &herds)
{
    /* set format attributes */
    output.set_maxlabel(all ? 11 : 15);
    output.set_maxdata(maxcol - output.maxlabel());
    output.set_attrs();

    /* was the all target specified? */
    if (all)
    {
        herds_xml.display(*stream);
        size = herds_xml.size();
        flush();
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
        if (eregex)
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

    /* for each specified herd... */
    opts_type::iterator herd;
    opts_type::size_type n = 1;
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
        if (not count and n != herds.size())
            output.endl();
    }

    flush();
    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
