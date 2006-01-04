/*
 * herdstat -- src/rc.cc
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

#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>

#include <herdstat/util/string.hh>
#include "common.hh"
#include "rc.hh"

#define HERDSTATRC_GLOBAL   SYSCONFDIR"/"PACKAGE"rc"
#define HERDSTATRC_LOCAL    /*HOME*/"/."PACKAGE"rc"

using namespace herdstat;

static bool
option_is_deprecated(const std::string& option)
{
    static const char *deprecated[] = {
        "querycache.expire",
        "querycache.max",
        "wget.options",
        "use.querycache"
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

rc::rc()
{
    if (util::is_file(HERDSTATRC_GLOBAL))
    {
        debug_msg("Reading configuration file %s", HERDSTATRC_GLOBAL);

        vars.read(HERDSTATRC_GLOBAL);
        set_options();
        vars.close();
    }

    const char * const homedir = std::getenv("HOME");
    if (homedir)
    {
        const std::string path(std::string(homedir) + HERDSTATRC_LOCAL);
        if (not util::is_file(path))
            return;

        debug_msg("Reading configuration file %s", path.c_str());

        vars.read(path);
        set_options();
        vars.close();
    }

    // show warnings for deprecated options
    std::for_each(vars.begin(), vars.end(), ShowDeprecatedOptionWarning());
}

void
rc::set_options()
{
    Options& options(GlobalOptions());

    if (not vars["colors"].empty())
        options.set_color(util::destringify<bool>(vars["colors"]));
    if (not vars["label_color"].empty())
	options.set_labelcolor(vars["label_color"]);
    if (not vars["highlight_color"].empty())
	options.set_hlcolor(vars["highlight_color"]);
    if (not vars["qa"].empty())
        options.set_qa(util::destringify<bool>(vars["qa"]));
    if (not vars["herdsxml"].empty())
	options.set_herdsxml(vars["herdsxml"]);
    if (not vars["gentoo_cvs"].empty())
        options.set_cvsdir(vars["gentoo_cvs"]);
    if (not vars["userinfo"].empty())
        options.set_userinfoxml(vars["userinfo"]);
    if (not vars["use_devaway"].empty())
        options.set_devaway(util::destringify<bool>(vars["use_devaway"]));
    if (not vars["devaway_expire"].empty())
        options.set_devaway_expire(util::destringify<long>(vars["devaway_expire"]));
    if (not vars["devaway_location"].empty())
        options.set_devawayxml(vars["devaway_location"]);
    if (not vars["use_metacache"].empty())
        options.set_metacache(util::destringify<bool>(vars["use_metacache"]));
    if (not vars["metacache_expire"].empty())
	options.set_metacache_expire(vars["metacache_expire"]);
    if (not vars["highlights"].empty())
        options.set_highlights(vars["highlights"]);
    if (not vars["frontend"].empty())
        options.set_iomethod(vars["frontend"]);
}

/* vim: set tw=80 sw=4 fdm=marker et : */
