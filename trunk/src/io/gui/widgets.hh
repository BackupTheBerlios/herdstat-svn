/*
 * herdstat -- src/io/gui/widgets.hh
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

#ifndef _HAVE_GUI_WIDGETS_HH
#define _HAVE_GUI_WIDGETS_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string>
#include <cstdlib>

#define HAS_TITLE \
    public: \
        void setTitle(const std::string& title) { _title.assign(title); } \
        const std::string& title() const { return _title; } \
    private: \
        std::string _title;

class Application
{
    public:
        Application() { }
        virtual ~Application() { }

        virtual void exec() = 0;
};

class Widget
{
    public:
        Widget() { }
        virtual ~Widget() { }
};

class Window
{
    HAS_TITLE

    public:
        Window() { }
        virtual ~Window() { }

        virtual void resize(std::size_t x, std::size_t y) = 0;
};

class Label
{
    HAS_TITLE

    public:
        Label() { }
        virtual ~Label() { }
};

class TextBox
{
    public:
        TextBox() { }
        virtual ~TextBox() { }
};

class MenuItem
{
    HAS_TITLE

    public:
        MenuItem() { }
        virtual ~MenuItem() { }
};

class Menu
{
    public:
        Menu() { }
        virtual ~Menu() { }
        virtual void addMenuItem(MenuItem *item) = 0;
};

class MenuBar
{
    public:
        MenuBar() { }
        virtual ~MenuBar() { }
        virtual void addMenu(Menu *menu) = 0;
};

class Tab
{
    HAS_TITLE

    public:
        Tab() { }
        virtual ~Tab() { }
};

class TabBar
{
    public:
        TabBar() { }
        virtual ~TabBar() { }
        virtual void addTab(Tab *tab) = 0;
};

class HBox
{
    public:
        HBox() { }
        virtual ~HBox() { }
};

class VBox
{
    public:
        VBox() { }
        virtual ~VBox() { }
};

#undef HAS_TITLE

#endif /* _HAVE_GUI_WIDGETS_HH */

/* vim: set tw=80 sw=4 et : */
