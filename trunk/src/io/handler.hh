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

#if defined(QT_FRONTEND) || defined(GTK_FRONTEND)
# include "io/gui/gui_factory.hh"
#endif

class IOHandler
{
    public:
        virtual ~IOHandler() { }
        virtual bool operator()(Query * const query) = 0;
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
};


#if defined(QT_FRONTEND) || defined(GTK_FRONTEND)
class GuiIOHandler : public IOHandler
{
    public:
        GuiIOHandler(int argc, char **argv);
        virtual ~GuiIOHandler() { }

        virtual bool operator()(Query * const query);

    protected:
        int _argc;
        char **_argv;

        herdstat::gui::GuiFactory *_guiFactory;
        Options& _options;
};
#endif /* QT_FRONTEND||GTK_FRONTEND */

#endif /* _HAVE_IO_HANDLER_HH */

/* vim: set tw=80 sw=4 et : */
