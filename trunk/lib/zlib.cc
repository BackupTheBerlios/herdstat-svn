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

#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "zlib.hh"

int
util::zlib_T::compress(const std::string &in, const std::string &out, int level)
{
    this->err = Z_OK;
    this->avail_in = 0;
    this->avail_out = buflen;
    this->next_out = outbuf;

    this->fin = std::fopen(in.c_str(), "rb");
    if (not this->fin)
        throw bad_fileobject_E(in);

    this->fout = std::fopen(out.c_str(), "wb");
    if (not this->fout)
        throw bad_fileobject_E(out);

    {
        struct stat s;
        if (fstat(fileno(this->fin), &s) != 0)
            throw bad_fileobject_E(in);
        this->len = s.st_size;
    }

    deflateInit(this, level);

    while (true)
    {
        if (not this->load())
            break;

        this->err = deflate(this, Z_NO_FLUSH);
        this->flush();

        if (this->err != Z_OK)
            break;
    }

    while (true)
    {
        this->err = deflate(this, Z_FINISH);
        if (not this->flush() or (this->err != Z_OK))
            break;
    }

    deflateEnd(this);

    if (this->err != Z_OK and this->err != Z_STREAM_END)
        throw zlib_E();
    else
        this->err = Z_OK;

    if (this->fin)
        std::fclose(fin);
    if (this->fout)
        std::fclose(fout);
    this->fin = this->fout = NULL;

    return this->err;
}

int
util::zlib_T::decompress(const std::string &in, const std::string &out)
{
    this->err = Z_OK;
    this->avail_in = 0;
    this->avail_out = buflen;
    this->next_out = outbuf;

    this->fin = std::fopen(in.c_str(), "rb");
    if (not this->fin)
        throw bad_fileobject_E(in);

    this->fout = std::fopen(out.c_str(), "wb");
    if (not this->fout)
        throw bad_fileobject_E(out);

    {
        struct stat s;
        if (fstat(fileno(this->fin), &s) != 0)
            throw bad_fileobject_E(in);
        this->len = s.st_size;
    }

    inflateInit(this);

    while (true)
    {
        if (not this->load())
            break;

        this->err = inflate(this, Z_NO_FLUSH);
        this->flush();

        if (this->err != Z_OK)
            break;
    }

    while (true)
    {
        this->err = inflate(this, Z_FINISH);
        if (not this->flush() or (this->err != Z_OK))
            break;
    }

    inflateEnd(this);

    if (this->err != Z_OK and this->err != Z_STREAM_END)
        throw zlib_E();
    else
        this->err = Z_OK;

    if (this->fin)
        std::fclose(fin);
    if (this->fout)
        std::fclose(fout);
    this->fin = this->fout = NULL;

    return this->err;
}

int
util::zlib_T::load()
{
    if (this->avail_in == 0)
    {
        this->next_in = inbuf;
        this->avail_in = std::fread(this->inbuf, 1, buflen, this->fin);
    }

    return this->avail_in;
}

int
util::zlib_T::flush()
{
    unsigned count = this->buflen - this->avail_out;

    if (count)
    {
        if (std::fwrite(this->outbuf, 1, count, this->fout) != count)
        {
            this->err = Z_ERRNO;
            return 0;
        }

        this->next_out = this->outbuf;
        this->avail_out = buflen;
    }

    return count;
}

#endif /* HAVE_LIBZ */

/* vim: set tw=80 sw=4 et : */
