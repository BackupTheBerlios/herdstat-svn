/*
 * herdstat -- src/action/keywords.cc
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

#include "action/keywords.hh"

using namespace herdstat;
using namespace gui;

const char * const
KeywordsActionHandler::id() const
{
    return "keywords";
}

const char * const
KeywordsActionHandler::desc() const
{
    return "Get keywords for the given packages.";
}

void
KeywordsActionHandler::operator()(const Query& query,
                                  QueryResults * const results)
{

}

Tab *
KeywordsActionHandler::createTab(GuiFactory *guiFactory)
{
    Tab *tab = guiFactory->createTab();
    tab->set_title(this->id());

    return tab;
}

/* vim: set tw=80 sw=4 et : */
