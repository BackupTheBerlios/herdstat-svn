/*
 * herdstat -- herdstat/util/timer.cc
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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <cstddef>
#include <herdstat/util/timer.hh>

namespace util {
/****************************************************************************/
timer_T::timer_T() : _elapsed(0), _running(false)
{
}
/****************************************************************************/
void
timer_T::start()
{
    if (this->_running)
        return;

    gettimeofday(&(this->_begin), NULL);
    
    this->_running = true;
}
/****************************************************************************/
void
timer_T::stop()
{
    if (not this->_running)
        return;

    gettimeofday(&(this->_end), NULL);
    size_type tmp(this->_elapsed);
    this->_elapsed = this->_end.tv_sec - this->_begin.tv_sec;
    this->_elapsed *= 1000;
    this->_elapsed += (this->_end.tv_usec - this->_begin.tv_usec) / 1000;
    this->_elapsed += tmp;

    this->_running = false;
}
/****************************************************************************/
} // namespace util

/* vim: set tw=80 sw=4 et : */
