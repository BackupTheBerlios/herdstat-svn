/*
 * herdstat -- src/pkgcache-dump.cc
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

#include "common.hh"
#include "formatter.hh"
#include "pkgcache.hh"

int
main(int argc, char **argv)
{
    util::color_map_T color;
    formatter_T out;
    out.set_colors(true);
    out.set_labelcolor(color[green]);
    out.set_highlightcolor(color[yellow]);
    out.add_highlight(util::current_user());

    pkgCache_T pkgcache;
    pkgcache.load();
    pkgcache.dump(&std::cout);
    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
