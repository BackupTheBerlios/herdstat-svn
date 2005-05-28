/*
 * herdstat -- lib/zlib.cc
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

#ifdef HAVE_LIBZ

#include "zlib.hh"

int
util::zlib_T::compress(const util::string &in, const util::string &out, int level)
{
    this->err = Z_OK;
    this->avail_in = 0;
    this->avail_out = buflen;
    this->next_out = outbuf;

    this->fin = fopen(in.c_str(), "rb");
    this->fout = fopen(out.c_str(), "wb");

    struct stat s;
    fstat(std::fileno(fin), &s);
    this->len = s.st_size;

    deflateInit(this, level);

    while (true)
    {
        if (not this->load())
            break;

        this->err = deflate(this, Z_NO_FLUSH);
        this->flush();

        if (this->err != Z_OK)
            break;

        this->progress(this->percent());
    }

    while (true)
    {
        this->err = deflate(this, Z_FINISH);
        if (not this->flush() or (this->err != Z_OK))
            break;
    }

    this->progress(this->percent());
    deflateEnd(this);

    if (this->err != Z_OK and this->err != Z_STREAM_END)
        this->status("Zlib error");
    else
    {
        this->status("Success");
        this->err = Z_OK;
    }

    std::fclose(this->fin);
    std::fclose(this->fout);
    this->fin = this->fout = NULL;

    return this->err;
}

#endif /* HAVE_LIBZ */

/* vim: set tw=80 sw=4 et : */
