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
#include <memory>
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
#include "metadatas.hh"

metadatas_T::metadatas_T(const std::string &p, const std::string &c)
    : portdir(p), cache_file(c)
{
    /* check cache
     *  - exists and not expired? - read it
     *  - otherwise get metadata list and cache it
     */

    _m.reserve(METADATA_RESERVE);
    
    if (cache_is_valid())
        read_cache();
    else
    {
        get_metadatas();
        write_cache();
    }
}

bool
metadatas_T::cache_is_valid()
{
    /* 
     * cache is considered valid if
     *  a) it exists
     *  b) it's not older than 24hrs
     *  c) it's size is >0 bytes
     */

    struct stat s;
    return ((stat(cache_file.c_str(), &s) == 0) and 
        ((time(NULL) - s.st_mtime) < 86400) and (s.st_size > 0));
}

/*
 * Fill vector with cached metadata.xml list
 */

void
metadatas_T::read_cache()
{
    std::auto_ptr<std::ifstream> cache(new std::ifstream(cache_file.c_str()));
    if (not (*cache))
        throw bad_fileobject_E(cache_file);

    std::string line;
    while (std::getline(*cache, line))
        _m.push_back(line);

    util::debug_msg("read a total of %d metadata.xml's from the cache",
        _m.size());
}

/*
 * Write cache with our current metadata.xml list
 */

void
metadatas_T::write_cache()
{
    std::auto_ptr<std::ofstream> cache(new std::ofstream(cache_file.c_str()));
    if (not (*cache))
        throw bad_fileobject_E(cache_file);

    for (iterator i = _m.begin() ; i != _m.end() ; ++i)
        *cache << (*i) << std::endl;
}

/*
 * Walk each category in the portage tree, searching for metadata.xml's
 */

void
metadatas_T::get_metadatas()
{
    util::progress_T progress;
    util::timer_T t;
    std::vector<std::string> categories = util::get_categories(portdir);
    bool status = not optget("quiet", bool) and not optget("debug", bool);
    bool timer  = optget("timer", bool);

    if (status)
    {
        *(optget("outstream", std::ostream *))
            << "Generating list of metadata.xml's: ";
        progress.start(categories.size());
    }

    if (timer)
        t.start();

    /* for each category */
    std::vector<std::string>::iterator cat;
    for (cat = categories.begin() ; cat != categories.end() ; ++cat)
    {
        std::string path = portdir + "/" + (*cat);

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
                _m.push_back(path + "/" + d->d_name + "/metadata.xml");
        }

        closedir(dir);
    }

    if (timer)
    {
        t.stop();
        util::debug_msg("Took %ldms to get %d metadata.xml's.",
            t.elapsed(), _m.size());
    }

    if (status)
    {
        *(optget("outstream", std::ostream *)) << " (total "
            << _m.size() << ")" << std::endl;
    }
}

/* vim: set tw=80 sw=4 et : */
