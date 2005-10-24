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

#include "options.hh"

using namespace herdstat;

std::ostream * options::_outstream = &std::cout;
std::string options::_outfile("stdout");

bool options::_init = false;
bool options::_verbose = false;
bool options::_quiet = false;
bool options::_debug = false;
bool options::_timer = false;
bool options::_all = false;
bool options::_dev = false;
bool options::_count = false;
bool options::_color = true;
bool options::_overlay = true;
bool options::_eregex = false;
bool options::_regex = false;
bool options::_qa = false;
bool options::_meta = false;
bool options::_metacache = true;
bool options::_querycache = true;
bool options::_devaway = true;

int options::_querycache_max = 100;
long options::_querycache_expire = 84600;
long options::_devaway_expire = 84600;
std::size_t options::_maxcol = 78;

std::string options::_cvsdir;
std::string options::_herdsxml;
std::string options::_devawayxml;
std::string options::_userinfoxml;
std::string options::_with_herd;
std::string options::_with_dev;
std::string options::_localstatedir(LOCALSTATEDIR);
std::string options::_wgetopts("-rq -t3 -T15");
std::string options::_labelcolor("green");
std::string options::_hlcolor("yellow");
std::string options::_metacache_expire("lastsync");
std::string options::_highlights;
std::string options::_locale(std::locale::classic().name());

fields_type options::_fields;

options_action options::_action = action_unspecified;

const std::string& options::_portdir = portage::GlobalConfig().portdir();
const std::vector<std::string>& options::_overlays = portage::GlobalConfig().overlays();

options::options()
{
    if (_init)
        return;

    _init = true;
}

/* vim: set tw=80 sw=4 et : */
