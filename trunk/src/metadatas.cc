/*
 * herdstat -- src/metadatas.cc
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

#include <fstream>
#include <algorithm>
#include <memory>
#include <iterator>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include "util.hh"
#include "options.hh"
#include "exceptions.hh"
#include "categories.hh"
#include "metadatas.hh"

metadatas_T::metadatas_T(const std::string &p)
    : cache_T<std::string>(CACHE), portdir(p)
{
    /* check cache
     *  - exists and not expired? - read it
     *  - otherwise get metadata list and cache it
     */
    
    _cache.reserve(METADATA_RESERVE);
    
    if (this->valid())
    {
        this->read();
        util::debug_msg("read %d metadata.xml's from the cache.", this->size());
    }
    else
    {
        this->fill();
        this->write();
        util::debug_msg("cached %d metadata.xml's.", this->size());
    }
}

/*
 * Determine whether the metadata cache is valid.
 */

bool
metadatas_T::valid() const
{
    struct stat s;
    bool valid = false;

    if (stat(CACHE, &s) == 0)
    {
        /* If the user's PORTDIR has a metadata/timestamp, it's been rsync'd
         * so only invalidate the cache when the timestamp's md5sum changes. */

        const std::string path = portdir + "/metadata/timestamp";
        bool timestamp = util::is_file(path);
        bool lastsync  = util::is_file(LASTSYNC);

        if (timestamp and lastsync)
        {
            if (util::md5check(path, LASTSYNC))
                valid = true;
            else
                util::copy_file(path, LASTSYNC);
        }
        /* no timestamp, so no rsync, just expire after 24hrs */
        else if (lastsync)
        {
            unlink(LASTSYNC);
            valid = ((time(NULL) - s.st_mtime) < 86400);
        }
        else if (timestamp)
            util::copy_file(path, LASTSYNC); 

        /* only valid if size > 0 */
        if (valid)
            valid = (s.st_size > 0);            
    }
    
    return valid;
}

/*
 * Walk each category in the portage tree, searching for package metadata.xml's,
 * filling our cache.
 */

void
metadatas_T::fill()
{
    categories_T categories(portdir);
    util::progress_T progress;
    util::timer_T t;
    bool status = not optget("quiet", bool) and not optget("debug", bool);
    bool timer  = optget("timer", bool);

    if (status)
    {
        *(optget("outstream", std::ostream *))
            << "Generating metadata.xml cache: ";
        progress.start(categories.size());
    }

    if (timer)
        t.start();

    /* for each category */
    categories_T::iterator cat;
    for (cat = categories.begin() ; cat != categories.end() ; ++cat)
    {
        const std::string path = portdir + "/" + (*cat);

        /* open category */
        DIR *dir = opendir(path.c_str());
        if (not dir)
            continue;

        if (status)
            ++progress;

        util::debug_msg("searching %s", path.c_str());

        struct dirent *d = NULL;
        while ((d = readdir(dir)))
        {
            /* skip anything starting with a '.' */
            if (std::strncmp(d->d_name, ".", 1) == 0)
                continue;

            /* instead of walking each directory, comparing d->d_name to
             * "metadata.xml", just stat the dir/metadata.xml */
            if (util::is_file(path + "/" + d->d_name + "/metadata.xml"))
                _cache.push_back(path + "/" + d->d_name + "/metadata.xml");
        }

        closedir(dir);
    }

    if (timer)
    {
        t.stop();
        util::debug_msg("Took %ldms to get %d metadata.xml's.",
            t.elapsed(), this->size());
    }

    if (status)
    {
        *(optget("outstream", std::ostream *)) << " (total "
            << this->size() << ")" << std::endl;
    }
}

/* vim: set tw=80 sw=4 et : */
