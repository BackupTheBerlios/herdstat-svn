/*
 * herdstat -- portage/ebuild.hh
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

#ifndef _HAVE_EBUILD_HH
#define _HAVE_EBUILD_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/**
 * @file ebuild.hh
 * @brief Defines the ebuild_T class.
 */

#include <herdstat/util/vars.hh>

namespace portage {

    /**
     * Represents ebuild variables.  This is really identical to vars_T,
     * except it re-defines set_defaults() and inserts variables that should
     * be pre-existing (${PN}, ${P}, etc).
     */

    class ebuild_T : public util::vars_T
    {
        public:
            ebuild_T();
            ebuild_T(const std::string &path);
            virtual ~ebuild_T() { }

            virtual void set_defaults();
    };

} // namespace portage

#endif /* _HAVE_EBUILD_HH */

/* vim: set tw=80 sw=4 et : */
