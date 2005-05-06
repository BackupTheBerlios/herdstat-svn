/*
 * herdstat -- src/action_handler.hh
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

#ifndef HAVE_ACTION_HANDLER_H
#define HAVE_ACTION_HANDLER_H 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <ostream>
#include "common.hh"
#include "herds_xml.hh"
#include "formatter.hh"

/*
 * Base action handler classes.
 */

class action_handler_T
{
    public:
        action_handler_T() : stream(optget("outstream", std::ostream *)),
                             config(optget("portage.config", portage::config_T)),
                             portdir(config.portdir()),
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
        const util::string portdir;         /* PORTDIR */
        std::size_t size;                   /* number of results */

        const bool quiet, verbose, regex, eregex,
                   all, debug, timer;
        bool count;
};

/* 
 * fancy (uses an output formatter)
 */

class action_fancy_handler_T : public action_handler_T
{
    public:
        action_fancy_handler_T() : action_handler_T(),
                                   maxcol(optget("maxcol", std::size_t)) { }
        virtual ~action_fancy_handler_T() { }

    protected:
        virtual void flush()
        {
            output.flush(*stream);
            action_handler_T::flush();            
        }

        formatter_T output;                 /* output formatter */
        const std::size_t maxcol;           /* columns of current terminal */
};

/*
 * fancy and parses herds.xml
 */

class action_herds_xml_handler_T : public action_fancy_handler_T
{
    public:
        action_herds_xml_handler_T() : action_fancy_handler_T() { }
        virtual ~action_herds_xml_handler_T() { }

    protected:
        virtual void flush()
        {
            action_fancy_handler_T::flush();

            if (timer)
                *stream << std::endl << "Took " << herds_xml.elapsed()
                    << "ms to parse herds.xml." << std::endl;
        }

        herds_xml_T herds_xml;
};

#endif

/* vim: set tw=80 sw=4 et : */
