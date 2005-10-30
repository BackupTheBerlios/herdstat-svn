/*
 * herdstat -- src/action/meta.cc
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

#include "action/meta.hh"

using namespace herdstat;
using namespace gui;

const char * const
MetaActionHandler::id() const
{
    return "meta";
}

const char * const
MetaActionHandler::desc() const
{
    return "Get metadata information for the given category/package.";
}

Tab *
MetaActionHandler::createTab(GuiFactory *guiFactory)
{
    Tab *tab = guiFactory->createTab();
    tab->set_title(this->id());

    return tab;
}

void
MetaActionHandler::operator()(const Query& query,
                              QueryResults * const results)
{

}

/* vim: set tw=80 sw=4 et : */
