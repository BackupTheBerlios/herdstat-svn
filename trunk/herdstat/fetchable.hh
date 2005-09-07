/*
 * herdstat -- herdstat/fetchable.hh
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

#ifndef _HAVE_FETCHABLE_HH
#define _HAVE_FETCHABLE_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/**
 * @file fetchable.hh
 * @brief Defines the abstract fetchable interface.
 */

#include <herdstat/fetcher.hh>

class fetchable
{
    public:
        virtual void fetch(const std::string& path = "") const
        {
            if (this->_fetched)
                return;
            this->do_fetch(path);
            this->_fetched = true;
        }

        bool fetched() const { return _fetched; }

    protected:
        fetchable() : _fetch(), _fetched(false) { }
        virtual ~fetchable() { }

        virtual void do_fetch(const std::string& path = "") const = 0;

        const fetcher _fetch;

    private:
        mutable bool _fetched;
};

#endif /* _HAVE_FETCHABLE_HH */

/* vim: set tw=80 sw=4 et : */
