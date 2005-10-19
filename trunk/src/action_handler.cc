/*
 * herdstat -- src/action_handler.cc
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

#include "action_handler.hh"

using namespace herdstat;

action_handler::action_handler()
    : stream(options::outstream()), regexp(), color(), size(0)
{
    regexp.set_cflags(options::eregex() ?
        util::Regex::extended|util::Regex::icase : util::Regex::icase);
}

void
action_handler::flush()
{
    if (options::count())
        *stream << size << std::endl;
}

action_fancy_handler::action_fancy_handler()
    : output(GlobalFormatter())
{
}

void
action_fancy_handler::flush()
{
    output.flush(*stream);
    action_handler::flush();

    if (output.attrs().marked_away() and not options::count())
    {
        *stream << std::endl << output.attrs().devaway_color()
            << "*" << color[none] << " Currently away" << std::endl;

        /* set false so the above isn't displayed more than once
         * in cases where more than one action handler is run. */
        output.attrs().set_marked_away(false);

        if (options::timer() and not options::count())
            *stream << std::endl << "Took " << devaway.elapsed()
                << "ms to parse devaway.xml." << std::endl;
    }
}

action_herds_xml_handler::action_herds_xml_handler()
    : herdsxml()
{
    herdsxml.set_cvsdir(options::cvsdir());
    herdsxml.set_force_fetch((options::action() == action_fetch));
}

void
action_herds_xml_handler::flush()
{
    action_fancy_handler::flush();

    if (options::timer() and not options::count())
        *stream << "Took " << herdsxml.elapsed()
            << "ms to parse herds.xml." << std::endl;
}

void
action_portage_find_handler::flush()
{
    action_fancy_handler::flush();

    if (options::timer() and not options::count())
        *stream << std::endl << "Took " << search_timer.elapsed()
            << "ms to perform search." << std::endl;
}

/* vim: set tw=80 sw=4 et : */
