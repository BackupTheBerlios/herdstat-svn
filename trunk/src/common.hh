/*
 * herdstat -- src/common.hh
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

#ifndef HAVE_COMMON_HH
#define HAVE_COMMON_HH 1

#include <string>
#include <vector>
#include <functional>

#include <herdstat/util/misc.hh>
#include <herdstat/portage/herds_xml.hh>
#include <herdstat/portage/devaway_xml.hh>
#include <herdstat/portage/userinfo_xml.hh>

#include "options.hh"
#include "exceptions.hh"
#include "query_results.hh"

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#define USE_XMLWRAPP
#define LASTSYNC        /*LOCALSTATEDIR*/"/lastsync"

#define NELEMS(x) (sizeof(x) / sizeof(x[0]))

typedef std::vector<std::string> opts_type;

void debug_msg(const char *, ...);
void fetch_devawayxml();
void fetch_herdsxml();

herdstat::portage::HerdsXML& GlobalHerdsXML();
herdstat::portage::DevawayXML& GlobalDevawayXML();
herdstat::portage::UserinfoXML& GlobalUserinfoXML();
herdstat::util::ColorMap& GlobalColorMap();

struct ColorAmbiguousPkg
    : std::binary_function<std::string, QueryResults * const, void>
{
    void operator()(const std::string& str, QueryResults * const results) const
    {
        Options& options(GlobalOptions());
        if (options.quiet() or not options.color())
            results->add(str);
        else
        {
            herdstat::util::ColorMap& color(GlobalColorMap());
            results->add(color[green] + str + color[none]);
        }
    }
};

#endif

/* vim: set tw=80 sw=4 fdm=marker et : */
