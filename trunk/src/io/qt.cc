/*
 * herdstat -- src/io/qt.cc
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

#include <qapplication.h>
#include <qtabbar.h>
#include <qfont.h>
#include <herdstat/noncopyable.hh>

#include "handler_map.hh"
#include "action/handler.hh"
#include "io/qt.hh"

using namespace herdstat;

class QtFrontEnd : private noncopyable
{
    public:
        QApplication& app() { return _app; }

    private:
        friend QtFrontEnd& GlobalQtFrontEnd(int = 0, char ** = NULL);
        QtFrontEnd(int argc, char **argv);
        ~QtFrontEnd();

        QApplication _app;
        QTabBar _tabs;
};

QtFrontEnd::QtFrontEnd(int argc, char **argv)
    : _app(argc, argv), _tabs()
{
    _tabs.resize(400, 400);

    /* insert a new tab for each action handler */
    HandlerMap<ActionHandler>& handlers(GlobalHandlerMap<ActionHandler>());
    HandlerMap<ActionHandler>::iterator i;
    for (i = handlers.begin() ; i != handlers.end() ; ++i)
        _tabs.addTab(new QTab(i->first));

    _app.setMainWidget(&_tabs);
    _tabs.show();
}

QtFrontEnd::~QtFrontEnd()
{
}

QtFrontEnd& GlobalQtFrontEnd(int argc, char **argv)
{
    static QtFrontEnd q(argc, argv);
    return q;
}

QtIOHandler::QtIOHandler(int argc, char **argv)
    : GuiIOHandler(argc, argv)
{
}

bool
QtIOHandler::input(Query * const query)
{
    QtFrontEnd& fe(GlobalQtFrontEnd(argc, argv));
    return (fe.app().exec());
}

bool
QtIOHandler::output(const QueryResults& results)
{
//    QtFrontEnd& fe(GlobalQtFrontEnd());
    return true;
}

/* vim: set tw=80 sw=4 et : */
