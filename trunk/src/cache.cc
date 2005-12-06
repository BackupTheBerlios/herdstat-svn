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
#include <herdstat/io/binary_stream.hh>
#include "cache.hh"

using namespace herdstat;

bool
PortageCacheHeader::is_valid(io::BinaryIStream& stream) const
{
    /* package cache header is in the form of:
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
    const std::string& npkgs(parts[3]);

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
    this->set_size(util::destringify<std::size_t>(npkgs));

    return true;
}

void
PortageCacheHeader::dump(io::BinaryOStream& stream, std::size_t size)
{
    this->set_size(size);
    stream << (std::string(VERSION)+":"+_options.portdir()+":"+
        util::join(_options.overlays().begin(),
                   _options.overlays().end(), ",")+":"+
        util::stringify(this->size()));
}

/* vim: set tw=80 sw=4 fdm=marker et : */
