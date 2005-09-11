/*
 * herdstat -- herdstat/fetcher.hh
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

#ifndef _HAVE_FETCHER_HH
#define _HAVE_FETCHER_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/**
 * @file fetcher.hh
 * @brief Defines the fetcher class.
 */

#include <string>
#include <herdstat/exceptions.hh>
#include <herdstat/noncopyable.hh>

/**
 * File fetcher.
 */

class Fetcher : private noncopyable
{
    public:
        /// Default constructor.
        Fetcher() : _opts() { }

        /** Constructor.
         * @param url Remote URL.
         * @param path Local path.
         */
        Fetcher(const std::string &url,
                const std::string &path) throw (FetchException);

        /** Fetch remote url, saving as local path.
         * @param url Remote URL.
         * @param path Local path.
         */
        void operator() (const std::string &url,
                         const std::string &path) const throw(FetchException);

        void set_options(const std::string &opts) const;

    private:
        int fetch(const std::string &url,
                  const std::string &path) const throw(FetchException);

        mutable std::string _opts;
};

inline void
Fetcher::set_options(const std::string &opts) const
{
    _opts.assign(opts);
}

#endif /* _HAVE_FETCHER_HH */

/* vim: set tw=80 sw=4 et : */
