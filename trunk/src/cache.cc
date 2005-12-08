/*
 * herdstat -- src/cache.cc
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

#include <herdstat/util/string.hh>
#include <herdstat/util/timer.hh>

#include "common.hh"
#include "cache.hh"

using namespace herdstat;

bool
Cache::Header::is_valid(io::BinaryIStream& stream)
{
    /* cache header is in the form of:
     *  <version>:<portdir>:<overlays (comma separated)>:<size>
     */

    std::string header;
    stream >> header;

    if (header.empty())
        return false;

    std::vector<std::string> parts;
    util::split(header, std::back_inserter(parts), ":", true);
    if (parts.size() != 4)
        return false;

    const std::string& version(parts[0]);
    const std::string& portdir(parts[1]);
    const std::string& overlays(parts[2]);
    const std::string& size(parts[3]);

    /* only valid if cached version is equal to our version */
    if (version != VERSION)
        return false;

    /* only valid if the cache came from the current portdir */
    if (portdir != _options.portdir())
        return false;

    /* likewise for overlays */
    std::vector<std::string> ovec;
    util::split(overlays, std::back_inserter(ovec), ",");
    if (ovec != _options.overlays())
        return false;

    /* number of packages cached */
    _size = util::destringify<std::size_t>(size);

    return true;
}

void
Cache::Header::dump(io::BinaryOStream& stream, std::size_t size)
{
    _size = size;
    stream << (std::string(VERSION)+":"+_options.portdir()+":"+
        util::join(_options.overlays().begin(),
                   _options.overlays().end(), ",")+":"+
        util::stringify(_size));
}

bool
Cache::is_valid()
{
    BacktraceContext c("Cache::is_valid("+_path+")");

    bool valid = false;

    if (this->do_is_valid())
    {
        _stream.open(_path);
        if (not _stream)
            throw FileException(_path);

        valid = _header.is_valid(_stream);

        /* if valid, keep stream open for load() to use */
        if (not valid)
            _stream.close();
    }

    debug_msg("%s cache is valid? %d",
        this->name(), valid);

    return valid;
}

void
Cache::fill()
{
    BacktraceContext c("Cache::fill("+_path+")");

    util::Timer timer;
    if (_options.timer())
        timer.start();

    this->do_fill();

    if (_options.timer())
    {
        timer.stop();
        _options.outstream() << "Took " << timer.elapsed()
            << "ms to fill the " << this->name() << " cache." << std::endl;
    }
}

void
Cache::load()
{
    BacktraceContext c("Cache::load("+_path+")");

    assert(_stream.is_open());

    util::Timer timer;
    if (_options.timer())
        timer.start();

    this->do_load(_stream);

    if (_options.timer())
    {
        timer.stop();
        _options.outstream() << "Took " << timer.elapsed()
            << "ms to load the " << this->name() << " cache." << std::endl;
    }

    _stream.close();
}

void
Cache::dump()
{
    BacktraceContext c("Cache::dump("+_path+")");

    assert(not _stream.is_open());

    io::BinaryOStream stream(_path);
    if (not stream)
        throw FileException(_path);

    _header.dump(stream, this->cache_size());

    this->do_dump(stream);
}

/* vim: set tw=80 sw=4 fdm=marker et : */
