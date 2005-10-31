/*
 * herdstat -- src/io/gui/gtk_widget_factory.cc
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
#include "io/gui/gtk_widget_factory.hh"

namespace gui {

/*
 * {{{ ConcreteProduct classes for GTK+ widgets
 */

class GtkWidget : public Widget
{
    public:
        GtkWidget(Gtk::Widget *widget = NULL);
        virtual ~GtkWidget();

    protected:
        Gtk::Widget *widget() { return _widget; }
        void set_widget(Gtk::Widget *widget) { _widget = widget; }

    private:
        Gtk::Widget *_widget;
};

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

class GtkTab : public Tab, public GtkWidget
{
    public:
        virtual ~GtkTab() { }
};

class GtkTabBar : public TabBar, public GtkWidget
{
    public:
        virtual ~GtkTabBar() { }
        virtual void add_tab(Tab *tab);

    private:
        Gtk::Notebook _bar;
};

/*
 * }}}
 */

GtkWidget::GtkWidget(Gtk::Widget *widget)
    : _widget(widget)
{
}

GtkWidget::~GtkWidget()
{
    if (_widget)
        delete _widget;
}

void
GtkTabBar::add_tab(Tab *tab)
{
    Gtk::VBox foo;
    _bar.append_page(foo, tab->title());
}

void
GtkApplication::exec()
{
    _app.run(_window);
}

Application *
GtkWidgetFactory::createApplication(int argc, char **argv) const
{
    return new GtkApplication(argc, argv);
}

Widget * GtkWidgetFactory::createWidget() const { return new GtkWidget(); }
Tab * GtkWidgetFactory::createTab() const { return new GtkTab(); }
TabBar * GtkWidgetFactory::createTabBar() const { return new GtkTabBar(); }

} // namespace gui

/* vim: set tw=80 sw=4 fdm=marker fdm=marker et : */
