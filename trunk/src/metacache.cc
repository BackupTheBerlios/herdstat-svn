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

#include "pkgcache.hh"
#include "metadata_xml.hh"
#include "metacache.hh"

#define METACACHE               LOCALSTATEDIR"/metacache"
#define METACACHE_EXPIRE        259200 /* 3 days */
#define METACACHE_RESERVE       8600

metacache_T::metacache_T(const util::string &portdir)
    : util::cache_T<value_type>(METACACHE), _portdir(portdir) { }

/*
 * Is the cache valid?
 */

bool
metacache_T::valid() const
{
    struct stat s;
    bool valid = false;

    const util::string expire(optget("metacache.expire", util::string));

    if (stat(METACACHE, &s) == 0)
    {
        if (expire == "lastsync")
        {
            const util::string path(this->_portdir + "/metadata/timestamp");
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
        }
        else
            valid = ((std::time(NULL) - s.st_mtime) <
                    std::strtol(expire.c_str(), NULL, 10));

        /* only valid if size > 0 */
        if (valid)
            valid = (s.st_size > 0);
    }

    debug_msg("metadata cache is valid? %d", valid);
    return valid;
}

/*
 * Find and parse every metadata.xml in the tree,
 * filling our container with data.
 */

void
metacache_T::fill()
{
    const bool status = not optget("quiet", bool) and not optget("debug", bool);
    {
        util::progress_T progress;
        pkgcache_T pkgcache(this->_portdir);
        debug_msg("pkgcache.size() == %d", pkgcache.size());

        if (status)
        {
            *(optget("outstream", std::ostream *))
                << "Generating metadata.xml cache: ";
            progress.start(pkgcache.size());
        }

        /* for each pkg */
        pkgcache_T::iterator p;
        for (p = pkgcache.begin() ; p != pkgcache.end() ; ++p)
        {
            const util::path_T path(this->_portdir + "/" + (*p));
            debug_msg("traversing %s...", path.c_str());
            if (*p == "xfce-extra/xfmedia")
            {
                debug_msg("found xfce-extra/xfmedia");

            }

            if (status)
                ++progress;

            if (not util::is_dir(path))
                continue;

            util::path_T metadata(path + "/metadata.xml");
            if (metadata.exists())
                this->push_back(this->parse(metadata));
        }
    }

    if (status)
        *(optget("outstream", std::ostream *)) << std::endl;
}

/*
 * Parse the given metadata.xml, and return a new metadata_T object.
 */

metadata_T
metacache_T::parse(const util::path_T &path)
{
    const metadata_xml_T meta(path);
    return meta.data(this->_portdir);
}

/*
 * Load cache from disk.  If a vector is given, only load lines
 * whose package is in the vector.
 */

void
metacache_T::load()
{
    if (not util::is_file(METACACHE))
        return;

    try
    {
        util::vars_T cache(METACACHE);
        this->_portdir = cache["portdir"];
        if (this->_portdir.empty())
            throw metacache_parse_E();

        if (not cache["size"].empty())
            this->reserve(std::atoi(cache["size"].c_str()));
        else
            this->reserve(METACACHE_RESERVE);

        util::vars_T::iterator i;
        for (i = cache.begin() ; i != cache.end() ; ++i)
        {
            /* not a category/package, so skip it */
            if (i->first.find('/') == util::string::npos)
                continue;

            util::string str;
            metadata_T meta(this->_portdir,
                this->_portdir + "/" + i->first + "/metadata.xml");

            std::vector<util::string> parts = i->second.split(':', true);
            if (parts.empty())
                throw metacache_parse_E();

            str = parts.front();
            parts.erase(parts.begin());
            meta.herds = str.split(',');

            if (not parts.empty())
            {
                str = parts.front();
                parts.erase(parts.begin());
                if (not str.empty())
                    meta.devs = str.split(',');
            }

            /* longdesc contains a ':', so reconstruct it */
            if (not parts.empty())
            {
                str = parts.front();
                parts.erase(parts.begin());

                while (not parts.empty())
                {
                    str += ":" + parts.front();
                    parts.erase(parts.begin());
                }

                meta.longdesc = str;
            }

            this->push_back(meta);
        }
    }
    catch (const metacache_parse_E)
    {
        std::cerr << "Error parsing " << METACACHE << std::endl;
        throw;
    }
}

/*
 * Dump cache to disk.
 */

void
metacache_T::dump()
{
    std::ofstream f(METACACHE);
    if (not f.is_open())
        throw util::bad_fileobject_E(METACACHE);

    f << "# Automatically generated by " << PACKAGE << "-" << VERSION
        << std::endl;
    f << "portdir=" << this->_portdir << std::endl;
    f << "size=" << this->size() << std::endl;

    /* for each metadata_T object */
    for (iterator ci = this->begin() ; ci != this->end() ; ++ci)
    {
        /*
         * format is the form of:
         *   cat/pkg=herd1,herd2:dev1,dev2:longdesc
         */

        util::string str;
        std::size_t n;

        f << ci->pkg << "=";

        metadata_T::herds_type::iterator h;
        for (h = ci->herds.begin(), n = 1 ; h != ci->herds.end() ; ++h, ++n)
        {
            str += (*h);
            if (n != ci->herds.size())
                str += ",";
        }

        f << str << ":";
        
        metadata_T::herd_type::iterator d;
        for (d = ci->devs.begin(), n = 1, str.clear() ; d != ci->devs.end() ; 
            ++d, ++n)
        {
            str += d->first;
            if (n != ci->devs.size())
                str += ",";
        }

        f << str << ":";

        if (ci->longdesc.empty())
            f << std::endl;
        else
        {
#ifdef UNICODE
            try
            {
                f << util::tidy_whitespace(ci->longdesc) << std::endl;
            }
            catch (const Glib::ConvertError)
            {
                f << std::endl;
            }
#else /* UNICODE */
            f << util::tidy_whitespace(ci->longdesc) << std::endl;
#endif /* UNICODE */
        }
    }
}

/* vim: set tw=80 sw=4 et : */
