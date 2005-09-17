/*
 * herdstat -- herdstat/portage/functional.hh
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

#ifndef _HAVE_PORTAGE_FUNCTIONAL_HH
#define _HAVE_PORTAGE_FUNCTIONAL_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/**
 * @file functional.hh
 * @brief portage-related function object definitions.
 */

#include <functional>
#include <herdstat/util/regex.hh>

namespace portage {

    struct Name
    {
        template <typename T>
        const std::string& operator()(const T& t) const { return t.name(); }
    };

    struct User
    {
        template <typename T>
        const std::string& operator()(const T& t) const { return t.user(); }
    };

    template <typename T>
    struct UserRegexMatch
        : std::binary_function<const util::regex_T *, T, bool>
    {
        bool operator()(const util::regex_T *re, const T &t) const
        { return (*re == t.user()); }
    };

    template <typename T>
    struct NameRegexMatch
        : std::binary_function<const util::regex_T *, T, bool>
    {
        bool operator()(const util::regex_T *re, const T& t) const
        { return (*re == t.name()); }
    };

} // namespace portage

#endif /* _HAVE_PORTAGE_FUNCTIONAL_HH */

/* vim: set tw=80 sw=4 et : */
