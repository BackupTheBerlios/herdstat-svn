/*
 * herdstat -- src/pkgcache.cc
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

#include <iostream>
#include <algorithm>
#include <iterator>

#include <herdstat/xml/exceptions.hh>
#include <herdstat/util/string.hh>
#include <herdstat/util/file.hh>
#include <herdstat/util/timer.hh>
#include <herdstat/portage/config.hh>
#include <herdstat/portage/categories.hh>

#include "common.hh"
#include "pkgcache.hh"

#define PKGCACHE                    /*LOCALSTATEDIR*/"/pkgcache"
#define PKGCACHE_EXPIRE             259200 /* 3 days */ 

using namespace herdstat;
using namespace herdstat::portage;
using namespace herdstat::xml;

pkgcache::pkgcache(const std::string &portdir)
    : Cachable(GlobalOptions().localstatedir()+PKGCACHE),
      _options(GlobalOptions()),
      _reserve(PKGLIST_RESERVE),
      _portdir(portdir), _overlays(_options.overlays()),
      _pkgs(_portdir, _overlays, false)
{
    this->logic();
}

pkgcache::~pkgcache()
{
}

/*
 * pkg cache is valid?
 */

bool
pkgcache::valid() const
{
    BacktraceContext c("pkgcache::valid()");

    const util::Stat pkgcache(this->path());
    bool valid = false;

    const std::string expire(_options.metacache_expire());
    const std::string lastsync(_options.localstatedir()+LASTSYNC);

    if (pkgcache.exists())
    {
        if (expire == "lastsync")
        {
            const std::string path(this->_portdir + "/metadata/timestamp");
            bool has_timestamp = util::is_file(path);
            bool has_lastsync  = util::is_file(lastsync);

            if (has_timestamp and has_lastsync)
            {
                util::File t(path), l(lastsync);
                valid = (t == l);
                if (not valid)
                {
                    debug_msg("timestamp != lastsync ; replacing lastsync file.");
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
        /* if expire option isn't "lastsync", treat it as a long integer */
        else
        {
            long ex(util::destringify<long>(expire));
            valid = ((std::time(NULL) - pkgcache.mtime()) < ex);
        }

        /* only valid if size > 0 */
        if (valid)
            valid = (pkgcache.size() > 0);
    }

    if (valid)
    {
        /*
         * read the first two lines checking portdir and overlays to see
         * if they're the same as what they were when the cache was created.
         */

        std::ifstream stream(this->path().c_str());
        if (not stream)
            throw FileException(this->path());

        /* only valid if saved portdir equals current portdir */
        std::string line;
        valid = (std::getline(stream, line) and
                (line == (std::string("portdir=")+this->_portdir)));

        /* only valid if saved overlays equal current overlays */
        if (valid)
        {
            valid = (std::getline(stream, line) and
                    (line == (std::string("overlays=") +
                              util::join(this->_overlays, ':'))));
        }
    }

    debug_msg("pkgcache is valid? %d", valid);
    return valid;
}

void
pkgcache::fill()
{
    BacktraceContext c("pkgcache::fill()");

    util::Timer timer;

    if (_options.timer())
        timer.start();

    _pkgs.fill();

    if (_options.timer())
    {
        timer.stop();
        _options.outstream() << "Took " << timer.elapsed()
            << "ms to fill the package cache." << std::endl;
    }
}

/*
 * Load package cache from disk.
 */

struct CacheEntryToPackage
{
    portage::Package
    operator()(const std::string& pkg) const
    {
        BacktraceContext c("CacheEntryToPackage::operator()("+pkg+")");
        std::vector<std::string> parts(util::split(pkg, ':'));
        if (parts.size() != 2)
            throw ParserException(GlobalOptions().localstatedir()+PKGCACHE, "Invalid format");
        return portage::Package(parts.front(), parts.back());
    }
};

struct PackageToCacheEntry
{
    std::string
    operator()(const Package& pkg) const
    {
        return (pkg.category()+"/"+pkg.name()+":"+pkg.portdir());
    }
};

void
pkgcache::load()
{
    BacktraceContext c("pkgcache::load("+this->path()+")");

    std::ifstream stream(this->path().c_str());
    if (not stream)
        throw FileException(this->path());

    /* ignore first two lines; it's just used for validating the cache */
    std::string line;
    std::getline(stream, line);
    std::getline(stream, line);
    
    /* get cached size for reserve() */
    std::getline(stream, line);
    std::string::size_type pos = line.find('=');
    if (pos != std::string::npos)
        _reserve = util::destringify<int>(line.substr(++pos));

    _pkgs.reserve(_reserve);
    std::transform(std::istream_iterator<std::string>(stream),
        std::istream_iterator<std::string>(), std::back_inserter(_pkgs),
        CacheEntryToPackage());
}

/*
 * Dump package cache to disk.
 */

void
pkgcache::dump()
{
    BacktraceContext c("pkgcache::dump("+this->path()+")");

    std::ofstream stream(this->path().c_str());
    if (not stream)
        throw FileException(this->path());

    /* this cache came from this->_portdir */
    stream << "portdir=" << this->_portdir << std::endl;
    stream << "overlays=" << util::join(this->_overlays, ':') << std::endl;
    stream << "size=" << _pkgs.size() << std::endl;

    std::transform(_pkgs.begin(), _pkgs.end(),
        std::ostream_iterator<std::string>(stream, "\n"),
        PackageToCacheEntry());
}

/* vim: set tw=80 sw=4 fdm=marker et : */
