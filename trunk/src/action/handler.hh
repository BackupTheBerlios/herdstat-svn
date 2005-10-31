/*
 * herdstat -- action/handler.hh
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

#ifndef _HAVE_ACTION_HANDLER_HH
#define _HAVE_ACTION_HANDLER_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <herdstat/util/misc.hh>

#include "query.hh"
#include "query_results.hh"
#include "io/gui/widget_factory.hh"

class ActionHandler
{
    public:
        virtual ~ActionHandler() { }

        /// this handler allows empty query objects to be passed?
        virtual bool allow_empty_query() const;

        /// action identifier string
        virtual const char * const id() const = 0;
        /// action description string
        virtual const char * const desc() const = 0;
        /// action usage string
        virtual const char * const usage() const;

        /// perform action
        virtual void operator()(const Query& query,
                                QueryResults * const results) = 0;

    protected:
        friend class GuiIOHandler;

        /* Called by GuiIOHandler::operator() when
         * filling the TabBar. */
        virtual gui::Tab *createTab(gui::WidgetFactory *factory) = 0;

        std::size_t size; /* for count option */
        herdstat::util::ColorMap color;
};

#endif /* _HAVE_ACTION_HANDLER_HH */

/* vim: set tw=80 sw=4 fdm=marker et : */
