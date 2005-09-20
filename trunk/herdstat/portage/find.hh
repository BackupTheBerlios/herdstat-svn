/*
 * herdstat -- herdstat/portage/portage_find.hh
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
#include <herdstat/util/regex.hh>
#include <herdstat/util/timer.hh>
#include <herdstat/portage/config.hh>

namespace portage
{
    /** Determine path to latest ebuild for specified package.
     * @param c  Reference to a portage::config_T object.
     * @param p  Package name.
     * @param o  Search overlays?
     * @param t  Pointer to a util::timer_T object (defaults to NULL).
     * @param pc Package cache (vector of package names).
     * @returns  A constant std::string object (path to ebuild).
     */
    const std::string
    ebuild_which(const config_T &c, const std::string &p,
        bool o = true, util::timer_T *t = NULL,
        const std::vector<std::string> &pc = std::vector<std::string>());

    /** Determine path to latest ebuild for specified package.
     * @param pd PORTDIR.
     * @param p  Package name.
     * @param t  Pointer to a util::timer_T object (defaults to NULL).
     * @param pc Package cache (vector of package names).
     * @returns  A constant std::string object (path to ebuild).
     */
    const std::string
    ebuild_which(const std::string &pd, const std::string &p,
        util::timer_T *t = NULL,
        const std::vector<std::string> &pc = std::vector<std::string>());
    
    /** Find full category/package specification for the specified package.
     * @param pd PORTDIR.
     * @param p  Package name.
     * @param t  Pointer to a util::timer_T object (defaults to NULL).
     * @param pc Package cache (vector of package names).
     * @returns  A std::string object (full category/path specification).
     */
    const std::string
    find_package_in(const std::string &pd, const std::string &p,
        util::timer_T *t = NULL,
        const std::vector<std::string> &pc = std::vector<std::string>());

    /** Find full category/package specification for the specified
     * regular expression.
     * @param pd  PORTDIR.
     * @param r   Reference to util::Regex object.
     * @param t   Pointer to util::timer_T object (Defaults to NULL).
     * @param pc  Package cache (vector of package names).
     * @returns   A vector of matching packages (in category/package form).
     */
    std::vector<std::string>
    find_package_regex_in(const std::string &pd, const util::Regex &r,
        util::timer_T *t = NULL,
        const std::vector<std::string> &pc = std::vector<std::string>());

    /** Find full category/package specification (and PORTDIR it is located
     * in) for the specified package name.
     * @param c  Reference to a portage::config_T object.
     * @param p  Package name.
     * @param o  Search overlays?
     * @param t  Pointer to util::timer_T object (Defaults to NULL).
     * @param pc Package cache (vector of package names).
     * @returns  A std::pair containing PORTDIR and category/package.
     */
    std::pair<std::string, std::string>
    find_package(const config_T &c, const std::string &p,
        bool o = true, util::timer_T *t = NULL,
        const std::vector<std::string> &pc = std::vector<std::string>());

    /** Find full category/package specification (and PORTDIR it is located
     * in) for the specified regular expression.
     * @param c  Reference to a portage::config_T object.
     * @param r  Reference to a util::Regex object.
     * @param o  Search overlays?
     * @param t  Pointer to util::timer_T object (Defaults to NULL).
     * @param pc Package cache (vector of package names).
     * @returns  A std::multimap containing PORTDIR and category/package for
     *           each match found.
     */
    std::multimap<std::string, std::string>
    find_package_regex(const config_T &c, const util::Regex &r,
        bool o = true, util::timer_T *t = NULL,
        const std::vector<std::string> &pc = std::vector<std::string>());
}

#endif

/* vim: set tw=80 sw=4 et : */
