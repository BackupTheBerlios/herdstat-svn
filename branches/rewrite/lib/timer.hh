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

#include <cstddef>
#include <sys/time.h>

namespace util
{
    /**
     * Represents a timer.
     */

    class timer
    {
        public:
            typedef long size_type;
            typedef struct timeval time_type;

            /// Default constructor.
            timer() : _elapsed(0), _running(false) { }

            /// Start timer.
            void start()
	    {
	        gettimeofday(&(this->_begin), NULL);
                this->_running = true;
	    }

            /// Stop timer.
	    void stop()
	    {
	        gettimeofday(&(this->_end), NULL);
                size_type tmp(this->_elapsed);
	        this->_elapsed = this->_end.tv_sec - this->_begin.tv_sec;
	        this->_elapsed *= 1000;
	        this->_elapsed +=
                    (this->_end.tv_usec - this->_begin.tv_usec) / 1000;
                this->_elapsed += tmp;
                this->_running = false;
	    }

            /** Is the timer running?
             * @returns A boolean value.
             */
            bool is_running() const { return this->_running; }

            /** How long has the timer been running?
             * @returns An long integer value.
             */
            size_type elapsed() const { return this->_elapsed; }

            /// Reset elapsed value.
            void reset() { this->_elapsed = 0; }

        private:
	    time_type _begin, _end;
	    size_type _elapsed;
            bool _running;
    };
}

#endif

/* vim: set tw=80 sw=4 et : */
