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
        IOHandler();
        virtual ~IOHandler();
        virtual bool operator()(Query * const query) = 0;

    protected:
        /* insert a local handler */
        template <typename Handler>
        inline void insert_local_handler(const std::string& name);
        /* get a local handler */
        inline ActionHandler *
        local_handler(const std::string& name);
        /* is it in our local handler map? */
        inline bool is_local_handler(const std::string& name) const;

        void init_xml_if_necessary(const std::string& action);

    private:
        HandlerMap<ActionHandler> _local;
};

template <typename Handler>
inline void
IOHandler::insert_local_handler(const std::string& name)
{
    ActionHandler *h = new Handler();
    if (not _local.insert(std::make_pair(name, h)).second)
        delete h;
}

inline ActionHandler *
IOHandler::local_handler(const std::string& name)
{
    return _local[name];
}

inline bool
IOHandler::is_local_handler(const std::string& name) const
{
    return (_local.find(name) != _local.end());
}

#endif /* _HAVE_IO_HANDLER_HH */

/* vim: set tw=80 sw=4 fdm=marker et : */
