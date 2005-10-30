/*
 * herdstat -- src/io/gui/qt_widget_factory.hh
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

#ifndef _HAVE__QT_WIDGET_FACTORY_HH
#define _HAVE__QT_WIDGET_FACTORY_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/**
 * @file io/gui/qt_widget_factory.hh
 * @brief Defines a ConcreteFactory for creating Qt widgets
 */

#include "io/gui/widget_factory.hh"

namespace gui {

    class QtWidgetFactory : public WidgetFactory
    {
        public:
            virtual ~QtWidgetFactory() { }

            virtual Widget *createWidget() const;
            virtual Application *createApplication(int argc, char **argv) const;
            virtual Tab *createTab() const;
            virtual TabBar *createTabBar() const;
    };

} // namespace gui

#endif /* _HAVE__QT_WIDGET_FACTORY_HH */

/* vim: set tw=80 sw=4 fdm=marker et : */
