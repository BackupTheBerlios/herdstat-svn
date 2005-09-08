/*
 * herdstat -- src/action_handler.hh
 * $Id: action_handler.hh 508 2005-09-03 11:30:08Z ka0ttic $
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

#ifndef HAVE_ACTION_HANDLER_H
#define HAVE_ACTION_HANDLER_H 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <ostream>
#include <herdstat/portage/config.hh>
#include <herdstat/portage/herds_xml.hh>
#include <herdstat/portage/devaway_xml.hh>

#include "common.hh"
#include "formatter.hh"

/*
 * Base action handler classes.
 */

class action_handler_T
{
    public:
        action_handler_T() : stream(optget("outstream", std::ostream *)),
                             config(optget("portage.config", portage::config_T)),
                             portdir(optget("portdir", std::string)),
                             size(0),
                             quiet(optget("quiet", bool)),
                             verbose(optget("verbose", bool)),
                             regex(optget("regex", bool)),
                             eregex(optget("eregex", bool)), 
                             all(optget("all", bool)),
                             debug(optget("debug", bool)),
                             timer(optget("timer", bool)),
                             count(optget("count", bool))  { }

        virtual ~action_handler_T() { }
        virtual int operator() (opts_type &) { return EXIT_FAILURE; }

    protected:
        virtual void flush() { if (count) *stream << size << std::endl; }

        std::ostream *stream;               /* output stream */
        util::regex_T regexp;               /* regular expression */
        util::color_map_T color;            /* color map */
        portage::config_T config;           /* portage configuration */
        const std::string portdir;         /* PORTDIR */
        std::size_t size;                   /* number of results */

        const bool quiet, verbose, regex, eregex,
                   all, debug;
        bool timer, count;
};

/* 
 * fancy (uses an output formatter)
 */

class action_fancy_handler_T : public action_handler_T
{
    public:
        action_fancy_handler_T() : action_handler_T(),
                                   maxcol(optget("maxcol", std::size_t)),
                                   use_devaway(optget("devaway", bool)) { }
        virtual ~action_fancy_handler_T() { }

    protected:
        virtual void flush()
        {
            output.flush(*stream);
            action_handler_T::flush();            

            if (output.marked_away() and not count)
            {
                *stream << std::endl << output.devaway_color()
                    << "*" << color[none] << " Currently away"
                    << std::endl;

                /* set false so the above isn't displayed more than once
                 * in cases where more than one action handler is run */
                output.set_marked_away(false);
            }
        }

        portage::devaway_xml devaway;
        formatter_T output;                 /* output formatter */
        const std::size_t maxcol;           /* columns of current terminal */
        const bool use_devaway;
};

/*
 * fancy and parses herds.xml
 */

class action_herds_xml_handler_T : public action_fancy_handler_T
{
    public:
        virtual ~action_herds_xml_handler_T() { }

    protected:
        virtual void flush()
        {
            action_fancy_handler_T::flush();

            if (timer and not count)
                *stream << std::endl << "Took " << herdsxml.elapsed()
                    << "ms to parse herds.xml." << std::endl;
        }

        portage::herds_xml herdsxml;
};

/*
 * fancy and searches portage
 */

class action_portage_find_handler_T : public action_fancy_handler_T
{
    public:
        action_portage_find_handler_T() : action_fancy_handler_T(),
                                          overlay(optget("overlay", bool)) { }
        virtual ~action_portage_find_handler_T() { }

    protected:
        virtual void flush()
        {
            action_fancy_handler_T::flush();

            if (timer and not count)
                *stream << std::endl << "Took " << search_timer.elapsed()
                    << "ms to perform search." << std::endl;
        }

        std::multimap<std::string, std::string> matches;
        util::timer_T search_timer;
        const bool overlay;
};

#endif

/* vim: set tw=80 sw=4 et : */
