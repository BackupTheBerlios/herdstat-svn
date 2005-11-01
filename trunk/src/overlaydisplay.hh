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

#include "options.hh"

/*
 * Keeps track of overlays and assigns them numbers.
 * Displays the overlay list (in order) upon destruction.
 */

class QueryResults;

class OverlayDisplay
{
    public:
        typedef std::set<std::pair<std::string, std::size_t>,
                         herdstat::util::SecondLess> container_type;
        typedef container_type::size_type size_type;
        typedef container_type::value_type value_type;
        typedef container_type::iterator iterator;
        typedef container_type::const_iterator const_iterator;

        OverlayDisplay(QueryResults * const results);
        ~OverlayDisplay();

        // Get a "[N]" string where N is the number of the given overlay.
        std::string operator[](const std::string& k) const;
        // insert new overlay
        void insert(const std::string& overlay);

        bool empty() const { return _oset.empty(); }
        size_type size() const { return _oset.size(); }

    private:
        Options& _options;
        QueryResults * const _results;
        container_type _oset;
};

#endif /* HAVE_OVERLAYDISPLAY_HH */

/* vim: set tw=80 sw=4 fdm=marker et : */
