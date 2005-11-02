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

#include <iostream>
#include <locale>

#include <herdstat/portage/config.hh>
#include "options.hh"

using namespace herdstat;

Options::Options()
    : _verbose(false), _quiet(false), _debug(false), _timer(false),
      _dev(false), _count(false), _color(true), _overlay(true),
      _eregex(false), _regex(false), _qa(false), _meta(false),
      _metacache(true), _querycache(true), _devaway(true), _fetch(false),
      _querycache_max(100), _querycache_expire(84600), _devaway_expire(84600),
      _maxcol(79), _outstream(&std::cout), _outfile("stdout"),
      _localstatedir(LOCALSTATEDIR), _labelcolor("green"),
      _hlcolor("yellow"), _metacache_expire("lastsync"),
      _locale(std::locale::classic().name()),
      _prompt(PACKAGE"> "),
      _action("unspecified"),
      _iomethod("unspecified"),
      _portdir(portage::GlobalConfig().portdir()),
      _overlays(portage::GlobalConfig().overlays())
{
}

/* vim: set tw=80 sw=4 fdm=marker et : */
