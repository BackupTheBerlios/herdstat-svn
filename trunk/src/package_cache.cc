/*
 * herdstat -- src/package_cache.cc
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
#include <herdstat/xml/exceptions.hh>
#include <herdstat/io/binary_stream_iterator.hh>

#include "common.hh"
#include "package_cache.hh"

#define PKGCACHE  /*LOCALSTATEDIR*/"/pkgcache"
#define PKGCACHE_EXPIRE  259200 /* 3 days */

using namespace herdstat;
using namespace herdstat::xml;

PackageCache::PackageCache()
    : Cache(GlobalOptions().localstatedir()+PKGCACHE),
      _portdir(_options.portdir()), _overlays(_options.overlays()),
      _pkgs(_portdir, _overlays, false)
{
    if (this->is_valid())
        this->load();
    else
    {
        this->fill();
        this->dump();
    }
}

PackageCache::~PackageCache() throw()
{
}

const char * const
PackageCache::name() const
{
    return "package";
}

std::size_t
PackageCache::cache_size() const
{
    return _pkgs.size();
}

bool
PackageCache::do_is_valid()
{
    BacktraceContext c("PackageCache::is_valid()");

    const util::Stat pkgcache(this->path());
    bool valid = false;

    const std::string& expire(_options.metacache_expire());
    const std::string lastsync(_options.localstatedir()+LASTSYNC);

    if (pkgcache.exists())
    {
        if (expire == "lastsync")
        {
            const std::string path(_portdir+"/metadata/timestamp");
            bool has_timestamp = util::is_file(path);
            bool has_lastsync  = util::is_file(lastsync);

            if (has_timestamp and has_lastsync)
            {
                util::File t(path), l(lastsync);
                valid = (t == l);
                if (not valid)
                {
                    debug_msg("timestamp != lastsync ; replacing lastsync");
                    t.close(); l.close();
                    util::copy_file(path, lastsync);
                }
            }
            else if (has_lastsync)
            {
                unlink(lastsync.c_str());
                valid = ((std::time(NULL) - pkgcache.mtime()) < PKGCACHE_EXPIRE);
            }
            else if (has_timestamp)
                util::copy_file(path, lastsync);
            else
                valid = ((std::time(NULL) - pkgcache.mtime()) < PKGCACHE_EXPIRE);
        }
        /* otherwise, treat it as a long int */
        else
        {
            long ex = util::destringify<long>(expire);
            valid = ((std::time(NULL) - pkgcache.mtime()) < ex);
        }

        /* only valid if size > 0 */
        if (valid)
            valid = (pkgcache.size() > 0);
    }

    return valid;
}

void
PackageCache::do_fill()
{
    BacktraceContext c("PackageCache::fill()");
    _pkgs.fill();
}

struct CacheEntryToPackage
{
    portage::Package
    operator()(const std::string& entry) const
    {
        std::string::size_type pos = entry.find(':');
        if (pos == std::string::npos)
            throw ParserException(GlobalOptions().localstatedir()+PKGCACHE,
                                  "Invalid format: '"+entry+"'.");
        return portage::Package(entry.substr(0, pos), entry.substr(pos+1));
    }
};

void
PackageCache::do_load(herdstat::io::BinaryIStream& stream)
{
    _pkgs.reserve(this->header_size());
    std::transform(io::BinaryIStreamIterator<std::string>(stream),
                   io::BinaryIStreamIterator<std::string>(),
                   std::back_inserter(_pkgs),
                   CacheEntryToPackage());
}

struct PackageToCacheEntry
{
    std::string
    operator()(const portage::Package& pkg) const
    {
        return (pkg.full()+":"+pkg.portdir());
    }
};

void
PackageCache::do_dump(io::BinaryOStream& stream)
{
     BacktraceContext c("PackageCache::dump()");

     std::transform(_pkgs.begin(), _pkgs.end(),
        io::BinaryOStreamIterator<std::string>(stream),
        PackageToCacheEntry());
}

/* vim: set tw=80 sw=4 fdm=marker et : */
