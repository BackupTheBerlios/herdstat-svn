/*
 * herdstat -- src/io/gui/qt_widget_factory.cc
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

#include "io/gui/qt_widget_factory.hh"

namespace gui {

/*
 * {{{ ConcreteProduct classes for Qt widgets
 */

class QtWidget : public Widget
{
    public:
        virtual ~QtWidget() { }
};

class QtApplication : public Application
{
    public:
        QtApplication(int argc, char **argv)
            : _app(argc, argv) { }
        virtual ~QtApplication() { }

        virtual void exec();

    private:
        QApplication _app;
};

class QtTab : public Tab
{
    public:
        virtual ~QtTab() { }
        virtual void set_title(const std::string& title);

    private:
        QTab _tab;
};

class QtTabBar : public TabBar
{
    public:
        virtual ~QtTabBar() { }
        virtual void add_tab(Tab *tab);
        virtual void show();
        virtual void resize(std::size_t x, std::size_t y);

    private:
        QTabBar _bar;
};

/*
 * }}}
 */

void
QtApplication::exec()
{
    _app.exec();
}

void
QtTab::set_title(const std::string& title)
{
    Tab::set_title(title);
    _tab.setText(title);
}

void
QtTabBar::add_tab(Tab *tab)
{
    _bar.addTab(new QTab(tab->title()));
}

void
QtTabBar::show()
{
    _bar.show();
}

void
QtTabBar::resize(std::size_t x, std::size_t y)
{
    _bar.resize(x, y);
}

Application *
QtWidgetFactory::createApplication(int argc, char **argv) const
{
    return new QtApplication(argc, argv);
}

Widget * QtWidgetFactory::createWidget() const { return new QtWidget(); }
Tab * QtWidgetFactory::createTab() const { return new QtTab(); }
TabBar * QtWidgetFactory::createTabBar() const { return new QtTabBar(); }

} // namespace gui

/* vim: set tw=80 sw=4 fdm=marker et : */
