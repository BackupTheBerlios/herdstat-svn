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
 * Given a list of herds, display herd and developer
 * information.
 */

int
action_herd_handler_T::operator() (herds_T &herds_xml,
                                   std::vector<std::string> &herds)
{
    options_T options;
    util::color_map_T color;
    std::ostream *stream = options.outstream();

    /* set format attributes */
    formatter_T output;
    output.set_maxlabel(15);
    output.set_maxdata(options.maxcol() - output.maxlabel());
    output.set_colors(true);
    output.set_quiet(options.quiet());
    output.set_labelcolor(color[green]);
    output.set_attrs();

    /* was the all target specified? */
    if (herds[0] == "all")
        herds_xml.display(*stream);

    /* nope, so only display stats for specified herds */
    else
    {
        /* for each specified herd... */
        std::vector<std::string>::iterator herd;
        std::vector<std::string>::size_type n = 0;
        for (herd = herds.begin() ; herd != herds.end() ; ++herd)
        {
            /* does the herd exist? */
            if (not herds_xml.exists(*herd))
            {
                /* if the user specified more than one herd, then just print
                 * the error and keep going; otherwise, we want to exit with
                 * an error code */
                if (herds.size() > 1)
                {
                    std::cerr << color[red] << "Herd '" << *herd
                        << "' doesn't seem to exist." << color[none] << std::endl;
                    std::cerr << std::endl;
                    continue;
                }
                else
                {
                    std::cerr << "Herd '" << *herd << "' doesn't seem to exist."
                        << std::endl;
                    throw herd_E();
                }
            }

            herds_xml[*herd]->display(*stream);

            /* only skip a line if we're not displaying the last one */
            if (++n != herds.size())
            {
                if (options.quiet())
                    *stream << std::endl;
                else
                    output.endl();
            }
        }
    }
        
    if (not options.quiet())
        output.flush(*stream);

    if (options.timer())
        *stream << std::endl;

    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
