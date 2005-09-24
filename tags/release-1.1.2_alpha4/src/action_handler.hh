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
        action_handler_T();
        virtual ~action_handler_T() { }
        virtual int operator() (opts_type &) = 0;

    protected:
        virtual void flush();

        std::ostream *stream;       /* output stream */
        util::Regex regexp;         /* regular expression */
        util::color_map_T color;    /* color map */
        std::size_t size;           /* number of results (for --count) */
};

/* 
 * fancy (uses an output formatter)
 */

class action_fancy_handler_T : public action_handler_T
{
    public:
        virtual ~action_fancy_handler_T() { }

    protected:
        virtual void flush();

        portage::devaway_xml devaway;
        formatter_T output;                 /* output formatter */
};

/*
 * fancy and parses herds.xml
 */

class action_herds_xml_handler_T : public action_fancy_handler_T
{
    public:
        action_herds_xml_handler_T();
        virtual ~action_herds_xml_handler_T() { }

    protected:
        virtual void flush();

        portage::herds_xml herdsxml;
};

/*
 * fancy and searches portage
 */

class action_portage_find_handler_T : public action_fancy_handler_T
{
    public:
        virtual ~action_portage_find_handler_T() { }

    protected:
        virtual void flush();

        std::multimap<std::string, std::string> matches;
        util::timer_T search_timer;
};

#endif

/* vim: set tw=80 sw=4 et : */
