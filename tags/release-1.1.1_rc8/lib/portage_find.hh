/*
 * herdstat -- lib/portage_find.hh
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

#ifndef HAVE_PORTAGE_FIND_HH
#define HAVE_PORTAGE_FIND_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <vector>
#include <map>
#include <utility>
#include "regex.hh"
#include "timer.hh"

namespace portage
{
    /*************************************************************************/
    const util::path_T
    ebuild_which(const portage::config_T &, const util::string &,
        bool overlays = true, util::timer_T *t = NULL,
        const std::vector<util::string>& = std::vector<util::string>());
    /*************************************************************************/
    const util::path_T
    ebuild_which(const util::string &, const util::string &,
        util::timer_T *t = NULL,
        const std::vector<util::string>& = std::vector<util::string>());
    /*************************************************************************/
    const util::string
    find_package_in(const util::string &, const util::string &,
        util::timer_T *t = NULL,
        const std::vector<util::string>& = std::vector<util::string>());
    /*************************************************************************/    
    std::vector<util::string>
    find_package_regex_in(const util::string &, const util::regex_T &,
        util::timer_T *t = NULL,
        const std::vector<util::string>& = std::vector<util::string>());
    /*************************************************************************/
    std::pair<util::string, util::string>
    find_package(const portage::config_T &, const util::string &,
        bool overlays = true, util::timer_T *t = NULL,
        const std::vector<util::string>& = std::vector<util::string>());
    /*************************************************************************/
    std::multimap<util::string, util::string>
    find_package_regex(const portage::config_T &, const util::regex_T &,
        bool overlays = true, util::timer_T *t = NULL,
        const std::vector<util::string>& = std::vector<util::string>());
    /*************************************************************************/
}

#endif

/* vim: set tw=80 sw=4 et : */
