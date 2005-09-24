/*
 * herdstat -- herdstat/util/progress.hh
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

#ifndef HAVE_PROGRESS_HH
#define HAVE_PROGRESS_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

namespace util {

    /**
     * Represents the amount of progress on an operation.
     */

    class progress_T
    {
	public:
            /// Default constructor.
	    progress_T();

            /// Destructor.
            ~progress_T();

            /** Start progress.
             * @param m Number of total items to process.
             */
	    void start(unsigned m);

            /// Increment progress.
	    inline void operator++ ();

	private:
            /// Current progress.
	    float _cur;
            /// Increment amount.
            float _step;
            /// Whether we've started yet.
            bool  _started;
    };

    inline void
    progress_T::operator++ ()
    {
        if (not this->_started)
            return;

        int inc = static_cast<int>(this->_cur += this->_step);
        if (inc < 10)       std::printf("\b\b%.1d%%", inc);
        else if (inc < 100) std::printf("\b\b\b%.2d%%", inc);
        else                std::printf("\b\b\b\b%.3d%%", inc);

        std::fflush(stdout);
    }

} // namespace util

#endif

/* vim: set tw=80 sw=4 et : */
