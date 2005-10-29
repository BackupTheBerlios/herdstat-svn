/*
 * herdstat -- src/io/gui/qt_factory.cc
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

#include "io/gui/qt_factory.hh"

namespace herdstat {
namespace gui {

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

class QtWidget : public Widget
{
    public:
        virtual ~QtWidget() { }

        void setWidget(QWidget *widget) { _widget = widget; }
        QWidget *widget() { return _widget; }

    private:
        QWidget *_widget;
};

class QtWindow : public Window
{
    public:
        virtual ~QtWindow() { }
        virtual void resize(std::size_t x, std::size_t y) { }
};

class QtButton : public Button
{
    public:
        virtual ~QtButton() { }
};

class QtLabel : public Label
{
    public:
        virtual ~QtLabel() { }
};

class QtMenuItem : public MenuItem
{
    public:
        virtual ~QtMenuItem() { }
};

class QtMenu : public Menu
{
    public:
        virtual ~QtMenu() { }
        virtual void addMenuItem(MenuItem *item) { }
};

class QtMenuBar : public MenuBar
{
    public:
        virtual ~QtMenuBar() { }
        virtual void addMenu(Menu *menu) { }
};

class QtTab : public Tab
{
    public:
        virtual ~QtTab() { }

    private:
        QTab _tab;
};

class QtTabBar : public TabBar
{
    public:
        virtual ~QtTabBar() { }
        virtual void addTab(Tab *tab);
        virtual void show();
        virtual void resize(std::size_t x, std::size_t y);

    private:
        QTabBar _bar;
};

class QtHBox : public HBox
{
    public:
        virtual ~QtHBox() { }
};

class QtVBox : public VBox
{
    public:
        virtual ~QtVBox() { }
};

void
QtApplication::exec()
{
    _app.exec();
}

void
QtTabBar::addTab(Tab *tab)
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
QtFactory::createApplication(int argc, char **argv) const
{
    return new QtApplication(argc, argv);
}

Window * QtFactory::createWindow() const { return new QtWindow(); }
Button * QtFactory::createButton() const { return new QtButton(); }
Label * QtFactory::createLabel() const { return new QtLabel(); }
MenuItem * QtFactory::createMenuItem() const { return new QtMenuItem(); }
Menu * QtFactory::createMenu() const { return new QtMenu(); }
MenuBar * QtFactory::createMenuBar() const { return new QtMenuBar(); }
Tab * QtFactory::createTab() const { return new QtTab(); }
TabBar * QtFactory::createTabBar() const { return new QtTabBar(); }
HBox * QtFactory::createHBox() const { return new QtHBox(); }
VBox * QtFactory::createVBox() const { return new QtVBox(); }

} // namespace gui
} // namespace herdstat

/* vim: set tw=80 sw=4 et : */
