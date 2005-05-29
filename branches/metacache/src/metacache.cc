/*
 * herdstat -- src/metacache.cc
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

#include "metacache_xml_handler.hh"
#include "metacache.hh"

#define METACACHE_UNCOMPRESSED  LOCALSTATEDIR"/metacache"
#define METACACHE_COMPRESSED    METACACHE_UNCOMPRESSED".zl"
#define METACACHE_EXPIRE        592200      /* one week */
#define METACACHE_RESERVE       8600
#define LASTSYNC                LOCALSTATEDIR"/lastsync"

#ifdef HAVE_LIBZ
# define METACACHE METACACHE_COMPRESSED
#else /* HAVE_LIBZ */
# define METACACHE METACACHE_UNCOMPRESSED
#endif /* HAVE_LIBZ */

class metacacheWriter
{

};

/*
 * metadata_T
 */

void
metadata_T::dump(const std::ostream &stream)
{

}

/*
 * metacache_T
 */

metacache_T::metacache_T() : cache_T<value_type>(METACACHE) { }

metacache_T::metacache_T(const string_type &p)
    : cache_T<value_type>(METACACHE), _portdir(p)
{
    this->_cache.reserve(METACACHE_RESERVE);
    this->init();
}

/*
 * Is the cache valid?
 */

bool
metacache_T::valid() const
{
    struct stat s;
    bool valid = false;

    if (stat(METACACHE, &s) == 0)
    {
        const util::string path(portdir + "/metadata/timestamp");
        bool timestamp = util::is_file(path);
        bool lastsync  = util::is_file(LASTSYNC);

        if (timestamp and lastsync)
        {
            valid = util::md5check(path, LASTSYNC);

            /* md5's don't match meaning the user has sync'd since last run */
            if (not valid)
                util::copy_file(path, LASTSYNC);
        }
        else if (lastsync)
        {
            unlink(LASTSYNC);
            valid = ((std::time(NULL) - s.st_mtime) < METACACHE_EXPIRE);
        }
        else if (timestamp)
            util::copy_file(path, LASTSYNC);
        else
            valid = ((std::time(NULL) - s.st_mtime) < METACACHE_EXPIRE);

        /* only valid if size > 0 */
        if (valid)
            valid = (s.st_size > 0);
    }

    debug_msg("metadata cache is valid? %d", valid);
    return valid;
}

void
metacache_T::fill()
{
    const portage::categories_T categories(this->_portdir, optget("qa", bool));
    util::progress_T progress;
    util::timer_T elapsed;
    const bool status = not optget("quiet", bool) and not optget("debug", bool);
    const bool timer = optget("timer", bool);

    if (status)
    {
        *(optget("outstream", std::ostream *))
            << "Generating metadata.xml cache: ";
        progress.start(categories.size());
    }

    if (timer)
        elapsed.start();

    /* for each category */
    protage::categories_T::const_iterator c;
    for (c = categories.begin() ; c != categories.end() ; ++c)
    {
        const util::path_T path(portdir + "/" + (*c));
        debug_msg("traversing %s...", path.c_str());

        if (status)
            ++progress;

        if (not util::is_dir(path))
            continue;

        /* parse category metadata.xml */
        if (util::is_file(path + "/metadata.xml"))
        {
            metadata_T meta = this->parse_metadata(path + "/metadata.xml");
            this->insert_metadata(meta);
        }

        const util::dir_T category(path);
        util::dir_T::const_iterator d;

        /* for each directory in this category */
        for (d = category.begin() ; d != category.end() ; ++d)
            if (util::is_file(*d + "/metadata.xml"))
                this->parse_metadata(*d + "/metadata.xml");
    }

    if (timer)
    {
        elapsed.stop();
        debug_msg("Took %ldms to process %d metadata.xml's.",
            elapsed.elapsed(), this->size());
    }
}

/*
 * Parse the given metadata.xml, create a new metadata_T
 * object and then insert it into the cache.
 */

metadata_T
metacache_T::parse_metadata(const util::path_T &path)
{
    const metadata_xml_T metadata(path);
    MetadataXMLHandler_T *handler = metadata.handler();

    /* TODO: just move metadata_T to metadata_xml_handler.hh?
     * and then just do a straight assignment */
    metadata_T meta(path);
    meta.herds = metadata.herds();
    meta.devs  = metadata.devs();
    meta.longdesc = metadata.longdesc();

    this->push_back(meta);
}

void
metacache_T::load()
{

}

void
metacache_T::dump()
{

}

/* vim: set tw=80 sw=4 et : */
