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

#include <cstdlib>
#include <cstring>
#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include "common.hh"
#include "metadatas.hh"

/*
 * Determine whether the metadata.xml cache is valid.
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
            valid = util::md5check(path, LASTSYNC);

            /* md5's don't match, meaning the user has sync'd since last run */
            if (not valid)
                util::copy_file(path, LASTSYNC);
        }
        /* no timestamp, so no rsync, just expire after 24hrs */
        else if (lastsync)
        {
            unlink(LASTSYNC);
            valid = ((time(NULL) - s.st_mtime) < DEFAULT_EXPIRE);
        }
        else if (timestamp)
            util::copy_file(path, LASTSYNC); 
        else
            valid = ((time(NULL) - s.st_mtime) < DEFAULT_EXPIRE);

        /* only valid if size > 0 */
        if (valid)
            valid = (s.st_size > 0);            
    }
    
    debug_msg("cache is valid? %d", valid);
    return valid;
}

/*
 * Walk each category in the portage tree, searching for package metadata.xml's,
 * filling our cache.
 */

void
metadatas_T::fill()
{
    portage::categories_T categories(portdir);
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
    portage::categories_T::iterator cat;
    for (cat = categories.begin() ; cat != categories.end() ; ++cat)
    {
        const std::string path(portdir + "/" + (*cat));
        debug_msg("searching %s", path.c_str());

        if (status)
            ++progress;

        util::dir_T category(path);
        util::dir_T::iterator d;

        /* for each directory in this category */
        for (d = category.begin() ; d != category.end() ; ++d)
        {
            /* instead of walking each directory, comparing d->d_name to
             * "metadata.xml", just stat the dir/metadata.xml */
            if (util::is_file(*d + "/metadata.xml"))
                _cache.push_back(*d + "/metadata.xml");
        }
    }

    if (timer)
    {
        t.stop();
        debug_msg("Took %ldms to get %d metadata.xml's.",
            t.elapsed(), this->size());
    }

    if (status)
    {
        *(optget("outstream", std::ostream *)) << " (total "
            << this->size() << ")" << std::endl;
    }
}

/* vim: set tw=80 sw=4 et : */
