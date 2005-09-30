/*
 * herdstat -- fetcher/options.hh
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

#ifndef _HAVE_OPTIONS_HH
#define _HAVE_OPTIONS_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/**
 * @file options.hh
 * @brief Defines the FetcherOptions class.
 */

class FetcherOptions
{
    public:
        FetcherOptions() : _verbose(false), _debug(false) { }

        inline bool verbose() const;
        inline bool debug() const;

        inline void set_verbose(bool v);
        inline void set_debug(bool v);

    private:
        bool _verbose;
        bool _debug;
};

inline bool FetcherOptions::verbose() const { return _verbose; }
inline bool FetcherOptions::debug() const { return _debug; }
inline void FetcherOptions::set_verbose(bool v) { _verbose = v; }
inline void FetcherOptions::set_debug(bool v) { _debug = v; }

#endif /* _HAVE_OPTIONS_HH */

/* vim: set tw=80 sw=4 et : */
