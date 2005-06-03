/*
 * herdstat -- lib/zlib.hh
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

#ifndef HAVE_ZLIB_HH
#define HAVE_ZLIB_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef HAVE_LIBZ

#include <cstdlib>
#include <zlib.h>

#include "string.hh"
#include "util_exceptions.hh"

/*
 * Simple zlib wrapper
 */

namespace util
{
    class zlib_E : public base_E { };

    class zlib_T : public z_stream
    {
        public:
            zlib_T() : fin(NULL), fout(NULL)
            {
                zalloc = NULL;
                zfree  = NULL;
                opaque = NULL;
            }

            int compress(const util::string &, const util::string &,
                int = Z_DEFAULT_COMPRESSION);
            int decompress(const util::string &, const util::string &);

        protected:
            int load();
            int flush();

            FILE *fin, *fout;
            long len;
            int err;

            enum { buflen = 4096 };
            unsigned char inbuf[buflen], outbuf[buflen];
    };
}

#endif /* HAVE_LIBZ */

#endif /* HAVE_ZLIB_HH */

/* vim: set tw=80 sw=4 et : */
