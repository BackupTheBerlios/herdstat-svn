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
#include <algorithm>
#include <iterator>
#include <locale>

#include <herdstat/util/string.hh>
#include <herdstat/portage/config.hh>
#include "options.hh"

#define HERDSTATRC_GLOBAL   SYSCONFDIR"/"PACKAGE"rc"
#define HERDSTATRC_LOCAL    /*HOME*/"/."PACKAGE"rc"

using namespace herdstat;

extern void debug_msg(const char *, ...);

static bool
option_is_deprecated(const std::string& option)
{
    static const char * const deprecated[] =
    {
        "querycache.expire",
        "querycache.max",
        "use.querycache",
        "wget.options"
    };

    return std::binary_search(deprecated,
            deprecated + NELEMS(deprecated), option);
}

struct ShowDeprecatedOptionWarning
{                   
    void operator()(const util::Vars::value_type& v) const
    {
        if (option_is_deprecated(v.first))          
        {
            std::cerr << "Warning: option '" << v.first
                << "' is deprecated and will be ignored." << std::endl;
        }
        else if (v.first.find('.') != std::string::npos)
        {
            std::cerr << "Warning: option '" << v.first
                << "' is deprecated.  Use '";
     
            std::replace_copy(v.first.begin(), v.first.end(),
                std::ostream_iterator<char>(std::cerr, ""), '.', '_');
                       
            std::cerr << "' instead." << std::endl;
        }
    }
};

Options::Options()
    : _verbose(false), _quiet(false), _debug(false), _timer(false),
      _dev(false), _count(false), _color(true), _overlay(true),
      _eregex(false), _regex(false), _qa(false), _meta(false),
      _metacache(true), _devaway(true), _fetch(false),
      _spinner(true), _devaway_expire(84600),
      _maxcol(79), _outstream(&std::cout), _outfile("stdout"),
      _localstatedir(LOCALSTATEDIR), _labelcolor("green"),
      _hlcolor("yellow"), _metacache_expire("lastsync"),
      _locale(std::locale::classic().name()),
      _prompt(PACKAGE"> "),
      _action("unspecified"),
      _iomethod("stream"),
      _portdir(portage::GlobalConfig().portdir()),
      _overlays(portage::GlobalConfig().overlays())
{
}

void
Options::read_configs()
{
    util::Vars v;

    if (util::is_file(HERDSTATRC_GLOBAL))
    {
        debug_msg("Reading configuration file %s", HERDSTATRC_GLOBAL);
        v.read(HERDSTATRC_GLOBAL);
        set_options_from_config(v);
        v.close();
    }

    const char * const homedir = std::getenv("HOME");
    if (homedir)
    {
        const std::string path(std::string(homedir) + HERDSTATRC_LOCAL);
        if (not util::is_file(path))
            return;

        debug_msg("Reading configuration file %s", path.c_str());

        v.read(path);
        set_options_from_config(v);
        v.close();
    }

    // show warnings for any deprecated options
    std::for_each(v.begin(), v.end(), ShowDeprecatedOptionWarning());
}

void
Options::set_options_from_config(util::Vars& vars)
{
    if (not vars["colors"].empty())
        set_color(util::destringify<bool>(vars["colors"]));
    if (not vars["label_color"].empty())
	set_labelcolor(vars["label_color"]);
    if (not vars["highlight_color"].empty())
	set_hlcolor(vars["highlight_color"]);
    if (not vars["qa"].empty())
        set_qa(util::destringify<bool>(vars["qa"]));
    if (not vars["herdsxml"].empty())
	set_herdsxml(vars["herdsxml"]);
    if (not vars["gentoo_cvs"].empty())
        set_cvsdir(vars["gentoo_cvs"]);
    if (not vars["userinfo"].empty())
        set_userinfoxml(vars["userinfo"]);
    if (not vars["use_devaway"].empty())
        set_devaway(util::destringify<bool>(vars["use_devaway"]));
    if (not vars["devaway_expire"].empty())
        set_devaway_expire(util::destringify<long>(vars["devaway_expire"]));
    if (not vars["devaway_location"].empty())
        set_devawayxml(vars["devaway_location"]);
    if (not vars["use_metacache"].empty())
        set_metacache(util::destringify<bool>(vars["use_metacache"]));
    if (not vars["metacache_expire"].empty())
	set_metacache_expire(vars["metacache_expire"]);
    if (not vars["highlights"].empty())
        set_highlights(vars["highlights"]);
    if (not vars["frontend"].empty())
        set_iomethod(vars["frontend"]);
}

/* vim: set tw=80 sw=4 fdm=marker et : */
