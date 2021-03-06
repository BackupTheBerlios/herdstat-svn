/*
 * herdstat -- src/io/gui.cc
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

#include "exceptions.hh"
#include "handler_map.hh"
#include "action/handler.hh"
#ifdef QT_FRONTEND
# include "io/gui/qt_widget_factory.hh"
#endif
#ifdef GTK_FRONTEND
# include "io/gui/gtk_widget_factory.hh"
#endif
#include "io/gui/widget_factory.hh"
#include "io/gui.hh"

using namespace gui;

GuiIOHandler::GuiIOHandler(int argc, char **argv)
    : _argc(argc), _argv(argv),
      _widgetFactory(NULL), _options(GlobalOptions())
{
#if defined(QT_FRONTEND) && defined(GTK_FRONTEND)
    if (_options.iomethod() == "qt")
        _widgetFactory = new QtWidgetFactory();
    else
        _widgetFactory = new GtkWidgetFactory();
#elif defined(QT_FRONTEND)
    _widgetFactory = new QtWidgetFactory();
#elif defined(GTK_FRONTEND)
    _widgetFactory = new GtkWidgetFactory();
#endif /* QT_FRONTEND && GTK_FRONTEND */

#if defined(QT_FRONTEND) || defined(GTK_FRONTEND)
    assert(_widgetFactory);
#endif
}

bool
GuiIOHandler::operator()(Query * const query)
{
    /* instantiate widgets */
    std::auto_ptr<Application> app(_widgetFactory->createApplication(_argc, _argv));
    std::auto_ptr<TabBar> tabs(_widgetFactory->createTabBar());

    /* setup user interface */
    

    /* have each action handler create their tab */
    HandlerMap<ActionHandler>& handlers(GlobalHandlerMap<ActionHandler>());
    HandlerMap<ActionHandler>::iterator i;
    for (i = handlers.begin() ; i != handlers.end() ; ++i)
    {
        Tab *tab = NULL;
        if ((tab = i->second->createTab(_widgetFactory)))
            tabs->add_tab(tab);
    }


    /* setup callbacks */

    /* when tab changes, call Query::set_action() with new tab name. */
    /* when action_button is pressed, execute action and display results */


    /* main event loop */

//    app->setMainWidget(tabs.get());
    tabs->resize(500, 400);
    tabs->show();
    app->exec();

    return false;
}

/* vim: set tw=80 sw=4 fdm=marker et : */
