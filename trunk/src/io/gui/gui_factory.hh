/*
 * herdstat -- src/io/gui/gui_factory.hh
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

#ifndef _HAVE_GUI_GUI_FACTORY_HH
#define _HAVE_GUI_GUI_FACTORY_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "io/gui/widgets.hh"

namespace herdstat {
namespace gui {

class GuiFactory
{
    public:
        GuiFactory() { }
        virtual ~GuiFactory() { }

        virtual Application *createApplication(int argc, char **argv) const = 0;
        virtual Window *createWindow() const = 0;
        virtual Label *createLabel() const = 0;
        virtual MenuItem *createMenuItem() const = 0;
        virtual Menu *createMenu() const = 0;
        virtual MenuBar *createMenuBar() const = 0;
        virtual Tab *createTab() const = 0;
        virtual TabBar *createTabBar() const = 0;
        virtual HBox *createHBox() const = 0;
        virtual VBox *createVBox() const = 0;
};

} // namespace gui
} // namespace herdstat

#endif /* _HAVE_GUI_GUI_FACTORY_HH */

/* vim: set tw=80 sw=4 et : */
