/*
 * herdstat -- src/io/gtk.cc
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
#include "io/gtk.hh"

class GtkFrontEnd
{
    public:
        void run() { _app.run(_main); }

    private:
        friend GtkFrontEnd& GlobalGtkFrontEnd(int argc = 0, char **argv = NULL);
        GtkFrontEnd(int argc, char **argv);
        ~GtkFrontEnd() { }

        Gtk::Main _app;
        Gtk::Window _main;
        Gtk::Notebook _notebook;
        Gtk::MenuBar _menu;
};

GtkFrontEnd::GtkFrontEnd(int argc, char **argv)
    : _app(argc, argv)
{
}

GtkFrontEnd& GlobalGtkFrontEnd(int argc, char **argv)
{
    static GtkFrontEnd g(argc, argv);
    return g;
}

GtkIOHandler::GtkIOHandler(int argc, char **argv)
    : GuiIOHandler(argc, argv)
{
}

bool
GtkIOHandler::operator()(Query * const query)
{
    GtkFrontEnd& fe(GlobalGtkFrontEnd(argc, argv));
    fe.run();
    return true;
}

/* vim: set tw=80 sw=4 et : */
