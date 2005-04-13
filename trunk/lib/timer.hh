/*
 * herdstat -- lib/timer.hh
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

#ifndef HAVE_TIMER_HH
#define HAVE_TIMER_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <cstdlib>
#include <sys/time.h>

namespace util
{
    class timer_T
    {
        private:
	    struct timeval _begin, _end;
	    long _ms;

        public:
            void start()
	    {
	        this->_ms = 0;
	        gettimeofday(&(this->_begin), NULL);
	    }

	    void stop()
	    {
	        gettimeofday(&(this->_end), NULL);
	        this->_ms = this->_end.tv_sec - this->_begin.tv_sec;
	        this->_ms *= 1000;
	        this->_ms += (this->_end.tv_usec - this->_begin.tv_usec) / 1000;
	    }

            long elapsed() const { return this->_ms; }
    };
}

#endif

/* vim: set tw=80 sw=4 et : */
