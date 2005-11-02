/*
 * herdstat -- io/handler.hh
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

#ifndef _HAVE_IO_HANDLER_HH
#define _HAVE_IO_HANDLER_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "query.hh"
#include "query_results.hh"
#include "formatter.hh"
#include "options.hh"
#include "handler_map.hh"
#include "action/handler.hh"

class IOHandler
{
    public:
        virtual ~IOHandler() { }

        virtual bool operator()(Query * const query) = 0;
        virtual void insert_extra_actions(HandlerMap<ActionHandler>&) const { }

    protected:
        void init_xml_if_necessary(const std::string& action);
};

class PrettyIOHandler : public IOHandler
{
    public:
        PrettyIOHandler();
        virtual ~PrettyIOHandler() { }

        void display(const QueryResults& results);

    protected:
        Formatter& out;
        FormatAttrs& attrs;
        Options& opts;
        herdstat::util::ColorMap& color;
};

#endif /* _HAVE_IO_HANDLER_HH */

/* vim: set tw=80 sw=4 fdm=marker et : */
