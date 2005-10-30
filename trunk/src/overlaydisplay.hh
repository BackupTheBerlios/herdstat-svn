/*
 * herdstat -- src/overlaydisplay.hh
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

#ifndef HAVE_OVERLAYDISPLAY_HH
#define HAVE_OVERLAYDISPLAY_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string>
#include <utility>
#include <herdstat/util/functional.hh>
#include <herdstat/util/container_base.hh>

#include "options.hh"

/*
 * Keeps track of overlays and assigns them numbers.
 * Displays the overlay list (in order) upon destruction.
 */

class OverlayDisplay
    // implemented-in-terms-of 'set<pair<string, size_t>, SecondLess>'
    : private herdstat::util::SetBase<
        std::pair<std::string, std::size_t>, herdstat::util::SecondLess>
{
    typedef herdstat::util::SetBase<std::pair<std::string, std::size_t>,
                                    herdstat::util::SecondLess> base_type;

    public:
        OverlayDisplay();
        virtual ~OverlayDisplay();

        // Get "[N]" string where N is the number of the given overlay.
        std::string operator[](const std::string& k);
        // Insert new overlay
        void insert(const std::string& overlay);

    private:
        Options& _options;
};

#endif /* HAVE_OVERLAYDISPLAY_HH */

/* vim: set tw=80 sw=4 fdm=marker et : */
