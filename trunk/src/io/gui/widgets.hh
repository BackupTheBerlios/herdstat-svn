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

#ifndef _HAVE__WIDGETS_HH
#define _HAVE__WIDGETS_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/**
 * @file io/gui/widgets.hh
 * @brief Abstract widget products for WidgetFactory.
 */

#include <string>
#include <herdstat/defs.hh>

namespace gui {

    class Widget
    {
        public:
            virtual ~Widget() { }
    };

    class WidgetWithTitle : public Widget
    {
        public:
            virtual ~WidgetWithTitle() { }
            virtual void set_title(const std::string& title);
            virtual const std::string& title() const;

        private:
            std::string _title;
    };

    class Application
    {
        public:
            virtual ~Application() { }
            virtual void exec() = 0;
    };

    class Tab : public WidgetWithTitle
    {
        public:
            virtual ~Tab() { }
    };

    class TabBar : public Widget
    {
        public:
            virtual ~TabBar() { }
            virtual void add_tab(Tab *tab) = 0;
            virtual void show() { }
            virtual void resize(std::size_t x LIBHERDSTAT_UNUSED,
                                std::size_t y LIBHERDSTAT_UNUSED) { }
    };

} // namespace gui

#endif /* _HAVE__WIDGETS_HH */

/* vim: set tw=80 sw=4 fdm=marker et : */
