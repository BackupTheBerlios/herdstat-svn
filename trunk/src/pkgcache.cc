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

#include "pkgcache.hh"

#define PKGCACHE                    LOCALSTATEDIR"/pkgcache"
#define PKGCACHE_EXPIRE             259200 /* 3 days */ 

pkgcache_T::pkgcache_T() : util::cache_T<value_type>(PKGCACHE) { }

pkgcache_T::pkgcache_T(const util::string &portdir)
    : util::cache_T<value_type>(PKGCACHE), _portdir(portdir)
{
    util::cache_T<value_type>::init();
}

void
pkgcache_T::init(const util::string &portdir)
{
    this->_portdir.assign(portdir);
    util::cache_T<value_type>::init();
}

/*
 * pkg cache is valid?
 */

bool
pkgcache_T::valid() const
{
    const util::stat_T pkgcache(PKGCACHE);
    bool valid = false;

    const util::string expire(optget("metacache.expire", util::string));

    if (pkgcache.exists())
    {
        if (expire == "lastsync")
        {
            const util::string path(this->_portdir + "/metadata/timestamp");
            bool timestamp = util::is_file(path);
            bool lastsync  = util::is_file(LASTSYNC);

            if (timestamp and lastsync)
            {
                util::file_T t(path), l(LASTSYNC);
                valid = (t == l);
                if (not valid)
                {
                    debug_msg("timestamp != lastsync ; replacing lastsync file.");
                    t.close(); l.close();
                    util::copy_file(path, LASTSYNC);
                }
            }
            else if (lastsync)
            {
                unlink(LASTSYNC);
                valid = ((std::time(NULL) - pkgcache.mtime()) < PKGCACHE_EXPIRE);
            }
            else if (timestamp)
                util::copy_file(path, LASTSYNC);
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

    if (optget("timer", bool))
        timer.start();

    const portage::categories_T categories(this->_portdir,
        optget("qa", bool));

    std::vector<util::string>::iterator i, e;
    std::vector<util::string> dirs = 
        optget("portage.config", portage::config_T).overlays();
    dirs.insert(dirs.begin(), this->_portdir);

    /* for each category */
    portage::categories_T::const_iterator c, ce = categories.end();
    for (c = categories.begin() ; c != ce ; ++c)
    {
        e = dirs.end();
        for (i = dirs.begin() ; i != e ; ++i)
        {
            const util::path_T cat(*i + "/" + (*c));
            if (not cat.exists())
                continue;

            /* for each directory in category */
            const util::dir_T category(cat);
            util::dir_T::const_iterator d , de = category.end();
            for (d = category.begin() ; d != de ; ++d)
            {
                util::string pkg(util::sprintf("%s/%s", c->c_str(), d->basename()));
                if ((*i == this->_portdir) or 
                    (std::find(this->begin(), this->end(), pkg) == this->end()))
                    this->push_back(pkg);
            }
        }
    }

    if (optget("timer", bool))
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
    std::ifstream stream(PKGCACHE);
    if (not stream.is_open())
        throw util::bad_fileobject_E(PKGCACHE);

    std::copy(std::istream_iterator<util::string>(stream),
        std::istream_iterator<util::string>(), std::back_inserter(*this));
}

/*
 * Dump package cache to disk.
 */

void
pkgcache_T::dump()
{
    std::ofstream stream(PKGCACHE);
    if (not stream.is_open())
        throw util::bad_fileobject_E(PKGCACHE);

    std::copy(this->begin(), this->end(),
        std::ostream_iterator<util::string>(stream, "\n"));
}

/* vim: set tw=80 sw=4 et : */
