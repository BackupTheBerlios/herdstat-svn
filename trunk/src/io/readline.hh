/*
 * herdstat -- src/io/readline.hh
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

#ifndef _HAVE_IO_READLINE_HH
#define _HAVE_IO_READLINE_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "io/pretty.hh"

/**
 * @class ReadLineIOHandler
 * @brief I/O handler for the readline front-end.
 */

class ReadLineIOHandler : public PrettyIOHandler
{
    public:
        /// Default constructor.
        ReadLineIOHandler();

        /// Destructor.
        virtual ~ReadLineIOHandler();

        /// handle input/display output
        virtual bool operator()(Query * const query);

    private:
        bool _read_hist;
        std::string _hist_path;
};

#endif /* _HAVE_IO_READLINE_HH */

/* vim: set tw=80 sw=4 fdm=marker et : */
