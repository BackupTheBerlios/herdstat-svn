/*
 * herdstat -- src/io/gui/gtk_factory.cc
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

#include <gtkmm.h>
#include "io/gui/gtk_factory.hh"

namespace herdstat {
namespace gui {

class GtkApplication : public Application
{
    public:
        GtkApplication(int argc, char **argv)
            : _app(argc, argv) { }
        virtual ~GtkApplication() { }

        virtual void exec();

    private:
        Gtk::Main _app;
        Gtk::Window _window;
};

void
GtkApplication::exec()
{
    _app.run(_window);
}

class GtkWindow : public Window
{
    public:
        virtual ~GtkWindow() { }
        virtual void resize(std::size_t x, std::size_t y) { }
};

class GtkButton : public Button
{
    public:
        virtual ~GtkButton() { }
};

class GtkLabel : public Label
{
    public:
        virtual ~GtkLabel() { }
};

class GtkMenuItem : public MenuItem
{
    public:
        virtual ~GtkMenuItem() { }
};

class GtkMenu : public Menu
{
    public:
        virtual ~GtkMenu() { }
        virtual void addMenuItem(MenuItem *item) { }
};

class GtkMenuBar : public MenuBar
{
    public:
        virtual ~GtkMenuBar() { }
        virtual void addMenu(Menu *menu) { }
};

class GtkTab : public Tab
{
    public:
        virtual ~GtkTab() { }
};

class GtkTabBar : public TabBar
{
    public:
        virtual ~GtkTabBar() { }
        virtual void addTab(Tab *tab) { }
};

class GtkHBox : public HBox
{
    public:
        virtual ~GtkHBox() { }
};

class GtkVBox : public VBox
{
    public:
        virtual ~GtkVBox() { }
};

Application *
GtkFactory::createApplication(int argc, char **argv) const
{
    return new GtkApplication(argc, argv);
}

Window * GtkFactory::createWindow() const { return new GtkWindow(); }
Button * GtkFactory::createButton() const { return new GtkButton(); }
Label * GtkFactory::createLabel() const { return new GtkLabel(); }
MenuItem * GtkFactory::createMenuItem() const { return new GtkMenuItem(); }
Menu * GtkFactory::createMenu() const { return new GtkMenu(); }
MenuBar * GtkFactory::createMenuBar() const { return new GtkMenuBar(); }
Tab * GtkFactory::createTab() const { return new GtkTab(); }
TabBar * GtkFactory::createTabBar() const { return new GtkTabBar(); }
HBox * GtkFactory::createHBox() const { return new GtkHBox(); }
VBox * GtkFactory::createVBox() const { return new GtkVBox(); }

} // namespace gui
} // namespace herdstat

/* vim: set tw=80 sw=4 et : */
