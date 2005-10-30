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

pkgcache::pkgcache()
    : cachable(GlobalOptions().localstatedir()+PKGCACHE),
      _options(GlobalOptions()),
      _reserve(PKGLIST_RESERVE),
      _portdir(_options.portdir()), _overlays(_options.overlays()),
      _pkgs(_portdir, _overlays)
{
}

pkgcache::pkgcache(const std::string &portdir)
    : cachable(GlobalOptions().localstatedir()+PKGCACHE),
      _options(GlobalOptions()),
      _reserve(PKGLIST_RESERVE),
      _portdir(portdir), _overlays(_options.overlays()),
      _pkgs(_portdir, _overlays)
{
    this->logic();
}

void
pkgcache::init()
{
    this->logic();
}

/*
 * pkg cache is valid?
 */

bool
pkgcache::valid() const
{
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
    util::Timer timer;

    if (_options.timer())
        timer.start();

    _pkgs.fill();

    if (_options.timer())
    {
        timer.stop();
        debug_msg("Took %ldms to fill package cache.", timer.elapsed());
    }
}

/*
 * Load package cache from disk.
 */

void
pkgcache::load()
{
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
    _pkgs.insert(_pkgs.end(),
        std::istream_iterator<std::string>(stream),
        std::istream_iterator<std::string>());
}

/*
 * Dump package cache to disk.
 */

void
pkgcache::dump()
{
    std::ofstream stream(this->path().c_str());
    if (not stream)
        throw FileException(this->path());

    /* this cache came from this->_portdir */
    stream << "portdir=" << this->_portdir << std::endl;
    stream << "overlays=" << util::join(this->_overlays, ':') << std::endl;
    stream << "size=" << _pkgs.size() << std::endl;

    std::copy(_pkgs.begin(), _pkgs.end(),
        std::ostream_iterator<std::string>(stream, "\n"));
}

/* vim: set tw=80 sw=4 fdm=marker et : */
