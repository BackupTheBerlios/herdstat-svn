/*
 * herdstat -- src/options.cc
 * $Id$
 * Copyright (c) 2005 Aaron Walker <ka0ttic at gentoo.org>
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

#include "options.hh"

bool options_T::_debug = false;
bool options_T::_timer = false;
bool options_T::_verbose = false;
bool options_T::_quiet = false;
std::string::size_type options_T::_maxcol = 78;
options_action_T options_T::_action = action_unspecified;
std::string options_T::_herds_xml = 
    "http://www.gentoo.org/cgi-bin/viewcvs.cgi/misc/herds.xml?rev=HEAD;cvsroot=gentoo;content-type=text/plain";
std::string options_T::_portdir = "/usr/portage";

/* vim: set tw=80 sw=4 et : */
