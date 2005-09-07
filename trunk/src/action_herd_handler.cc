/*
 * herdstat -- src/action_herd_handler.cc
 * $Id: action_herd_handler.cc 508 2005-09-03 11:30:08Z ka0ttic $
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
action_herd_handler_T::operator() (opts_type &opts)
{
    herdsxml.fetch(optget("herds.xml", std::string));
    herdsxml.parse(optget("herds.xml", std::string));
    const Herds& herds(herdsxml.herds());

    if (use_devaway)
    {
        devaway.fetch(optget("devaway.location", std::string));
        devaway.parse(optget("devaway.location", std::string));
    }

    /* set format attributes */
    output.set_maxlabel(all ? 11 : 15);
    output.set_maxdata(maxcol - output.maxlabel());
    if (use_devaway)
        output.set_devaway(devaway.keys());
    output.set_attrs();

    /* was the all target specified? */
    if (all)
    {
        display_herds(herds, *stream);
        size = herds.size();
        flush();
        return EXIT_SUCCESS;
    }
    else if (regex and opts.size() > 1)
    {
        std::cerr << "You may only specify one regular expression."
            << std::endl;
        return EXIT_FAILURE;
    }
    else if (regex)
    {
        if (eregex)
            regexp.assign(opts.front(), REG_EXTENDED|REG_ICASE);
        else
            regexp.assign(opts.front(), REG_ICASE);
        
        opts.clear();

        /* FIXME: use copy_if() ? */
        Herds::const_iterator h;
        for (h = herds.begin() ; h != herds.end() ; ++h)
        {
            if (regexp == h->name())
                opts.push_back(h->name());
        }
    }

    /* for each specified herd... */
    opts_type::iterator herd;
    opts_type::size_type n = 1;
    for (herd = opts.begin() ; herd != opts.end() ; ++herd, ++n)
    {
        /* does the herd exist? */
        Herds::const_iterator h = herds.find(*herd);
        if (h == herds.end())
        {
            std::cerr << "Herd '" << *herd << "' doesn't seem to exist."
                << std::endl;

            /* if the user specified more than one herd, then just print
             * the error and keep going; otherwise, we want to exit with
             * an error code */
            if (opts.size() > 1)
            {
                std::cerr << std::endl;
                continue;
            }
            else
                throw HerdException();
        }

        display_herd(*h, *stream);
        size += h->size();

        /* only skip a line if we're not displaying the last one */
        if (not count and n != opts.size())
            output.endl();
    }

    flush();
    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
