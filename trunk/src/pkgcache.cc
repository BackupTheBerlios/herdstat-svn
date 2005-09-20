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

#include <algorithm>
#include <iterator>

#include <herdstat/util/string.hh>
#include <herdstat/util/file.hh>
#include <herdstat/util/timer.hh>
#include <herdstat/portage/config.hh>
#include <herdstat/portage/categories.hh>
#include "pkgcache.hh"

#define PKGCACHE                    /*LOCALSTATEDIR*/"/pkgcache"
#define PKGCACHE_EXPIRE             259200 /* 3 days */ 

pkgcache_T::pkgcache_T()
    : cachable(options::localstatedir()+PKGCACHE),
      _portdir(options::portdir()),
      _overlays(options::overlays())
{
}

pkgcache_T::pkgcache_T(const std::string &portdir)
    : cachable(options::localstatedir()+PKGCACHE),
      _portdir(portdir),      
      _overlays(options::overlays())
{
    this->logic();
}

void
pkgcache_T::init()
{
    this->logic();
}

/*
 * pkg cache is valid?
 */

bool
pkgcache_T::valid() const
{
    const util::stat_T pkgcache(this->path());
    bool valid = false;

    const std::string expire(options::metacache_expire());
    const std::string lastsync(options::localstatedir()+LASTSYNC);

    if (pkgcache.exists())
    {
        if (expire == "lastsync")
        {
            const std::string path(this->_portdir + "/metadata/timestamp");
            bool has_timestamp = util::is_file(path);
            bool has_lastsync  = util::is_file(lastsync);

            if (has_timestamp and has_lastsync)
            {
                util::file_T t(path), l(lastsync);
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
        else
            valid = ((std::time(NULL) - pkgcache.mtime()) <
                    std::strtol(expire.c_str(), NULL, 10));

        /* only valid if size > 0 */
        if (valid)
            valid = (pkgcache.size() > 0);
    }

    if (valid)
    {
        std::ifstream stream(this->path().c_str());
        if (not stream)
            throw FileException(this->path());

        std::string line;
        valid = (std::getline(stream, line) and
                (line == (std::string("portdir=")+this->_portdir)));

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

/*
 * Traverse PORTDIR looking for all packages.
 */

void
pkgcache_T::fill()
{
    util::timer_T timer;

    if (options::timer())
        timer.start();

    const portage::Categories
        categories(this->_portdir, options::qa());

    std::vector<std::string> dirs(options::overlays());
    dirs.insert(dirs.begin(), this->_portdir);
    std::vector<std::string>::iterator i, e = dirs.end();

    /* for each category */
    portage::Categories::const_iterator c, ce = categories.end();
    for (c = categories.begin() ; c != ce ; ++c)
    {
        /* for each portdir */
        for (i = dirs.begin() ; i != e ; ++i)
        {
            const std::string cat(*i + "/" + (*c));
            if (not util::is_dir(cat))
                continue;

            /* for each directory in category */
            const util::dir_T category(cat);
            util::dir_T::const_iterator d , de = category.end();
            for (d = category.begin() ; d != de ; ++d)
            {
                std::string pkg(util::sprintf("%s/%s", c->c_str(), util::basename(*d)));
                if ((*i == this->_portdir) or 
                    (std::find(_pkgs.begin(), _pkgs.end(), pkg) == _pkgs.end()))
                    _pkgs.push_back(pkg);
            }
        }
    }

    if (options::timer())
    {
        timer.stop();
        debug_msg("Took %ldms to fill package cache.", timer.elapsed());
    }
}

/*
 * Load package cache from disk.
 */

void
pkgcache_T::load()
{
    std::ifstream stream(this->path().c_str());
    if (not stream)
        throw FileException(this->path());

    /* ignore first two lines, it's just used for validating the cache */
    std::string line;
    std::getline(stream, line);
    std::getline(stream, line);

//    std::copy(std::istream_iterator<std::string>(stream),
//        std::istream_iterator<std::string>(),
//        std::back_inserter(this->_pkgs));

    _pkgs.insert(_pkgs.end(),
        std::istream_iterator<std::string>(stream),
        std::istream_iterator<std::string>());
}

/*
 * Dump package cache to disk.
 */

void
pkgcache_T::dump()
{
    std::ofstream stream(this->path().c_str());
    if (not stream)
        throw FileException(this->path());

    /* this cache came from this->_portdir */
    stream << "portdir=" << this->_portdir << std::endl;
    stream << "overlays=" << util::join(this->_overlays, ':') << std::endl;

    std::copy(this->begin(), this->end(),
        std::ostream_iterator<std::string>(stream, "\n"));
}

/* vim: set tw=80 sw=4 et : */
