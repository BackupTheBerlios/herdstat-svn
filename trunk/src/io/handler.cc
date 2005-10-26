/*
 * herdstat -- io/handler.cc
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

#include <memory>
#include <herdstat/util/string.hh>

#ifdef QT_FRONTEND
# include "io/gui/qt_factory.hh"
#endif
#ifdef GTK_FRONTEND
# include "io/gui/gtk_factory.hh"
#endif

#include "exceptions.hh"
#include "handler_map.hh"
#include "action/handler.hh"
#include "io/handler.hh"

using namespace herdstat;

PrettyIOHandler::PrettyIOHandler()
    : out(GlobalFormatter()), attrs(out.attrs())
{
    const Options& opts(GlobalOptions());

    /* set common format attributes */
    attrs.set_maxlen(opts.maxcol());
    attrs.set_quiet(opts.quiet());
    attrs.set_colors(opts.color());

    /* add highlights */
    const std::string user(util::current_user());
    attrs.add_highlight(user);
    attrs.add_highlight(util::get_user_from_email(user));
    /* user-defined highlights */
    attrs.add_highlights(util::split(opts.highlights()));
}

void
PrettyIOHandler::display(const QueryResults& results)
{
    QueryResults::const_iterator i;
    for (i = results.begin() ; i != results.end() ; ++i)
        out(i->first, i->second);

    out.flush(GlobalOptions().outstream());
}

GuiIOHandler::GuiIOHandler(int argc, char **argv)
    : _argc(argc), _argv(argv),
      _guiFactory(NULL), _options(GlobalOptions())
{
#if defined(QT_FRONTEND) && defined(GTK_FRONTEND)
    if (_options.iomethod() == "qt")
        _guiFactory = new gui::QtFactory();
    else
        _guiFactory = new gui::GtkFactory();
#elif defined(QT_FRONTEND)
    _guiFactory = new gui::QtFactory();
#elif defined(GTK_FRONTEND)
    _guiFactory = new gui::GtkFactory();
#endif /* QT_FRONTEND && GTK_FRONTEND */

    assert(_guiFactory);
}

bool
GuiIOHandler::operator()(Query * const query)
{
    using namespace herdstat::gui;

    /* setup user interface */
    std::auto_ptr<Application> app(_guiFactory->createApplication(_argc, _argv));
    std::auto_ptr<HBox> hbox(_guiFactory->createHBox());
    std::auto_ptr<TabBar> tabs(_guiFactory->createTabBar());

    /* create a new tab for each action */
    HandlerMap<ActionHandler>& handlers(GlobalHandlerMap<ActionHandler>());
    HandlerMap<ActionHandler>::iterator i;
    for (i = handlers.begin() ; i != handlers.end() ; ++i)
        tabs->addTab(i->second->createTab(_guiFactory));

    /* construct query object with user input */

    /* perform action */
    ActionHandler *handler = handlers[query->action()];
    if (not handler)
        throw ActionUnimplemented(query->action());

    QueryResults results;
    (*handler)(*query, &results);

    /* show query results */

    /* main event loop */
    app->exec();

    /* clean up */

    return false;
}

/* vim: set tw=80 sw=4 et : */
