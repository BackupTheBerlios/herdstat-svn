/*
 * herdstat -- src/action_meta_handler.hh
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

#ifndef HAVE_ACTION_META_HANDLER_HH
#define HAVE_ACTION_META_HANDLER_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "overlaydisplay.hh"
#include "action_handler.hh"

class action_meta_handler_T : public action_fancy_handler_T
{
    public:
        action_meta_handler_T() : action_fancy_handler_T(),
                                  overlay(optget("overlay", bool)) { }
        virtual ~action_meta_handler_T() { }
        virtual int operator() (opts_type &);

    private:
        OverlayDisplay_T od;
        const bool overlay;
};

#endif

/* vim: set tw=80 sw=4 et : */
