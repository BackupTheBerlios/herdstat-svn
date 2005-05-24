/*
 * herdstat -- src/parsable.hh
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

#ifndef HAVE_PARSABLE_HH
#define HAVE_PARSABLE_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/*
 * Defines an abstract base class for parsable things.
 */

class parsable_T
{
    public:
        typedef util::path_T string_type;
        typedef util::timer_T timer_type;

        parsable_T() { }
        parsable_T(const string_type &p) : _path(p) { }
        virtual ~parsable_T() { }

        virtual void parse(const string_type & = "") = 0;

        string_type &path() { return this->_path; }
        timer_type::size_type elapsed() const
        { return this->_timer.elapsed(); }

    protected:
        virtual void init() = 0;
        virtual void fetch() { }

        string_type _path;              /* path to XML file */
        timer_type  _timer;             /* timer object */
};

#endif

/* vim: set tw=80 sw=4 et : */
