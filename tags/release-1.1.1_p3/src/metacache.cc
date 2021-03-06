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

#include <algorithm>
#include <functional>

#include "pkgcache.hh"
#include "metadata_xml.hh"
#include "metacache.hh"

#define METACACHE               /*LOCALSTATEDIR*/"/metacache"
#define METACACHE_EXPIRE        259200 /* 3 days */
#define METACACHE_RESERVE       8650

metacache_T::metacache_T(const util::string &portdir)
    : util::cache_T<value_type>(optget("localstatedir", util::string)+METACACHE),
      _portdir(portdir),
      _overlays(optget("portage.config", portage::config_T).overlays())
{

}

/*
 * Is the cache valid?
 */

bool
metacache_T::valid() const
{
    const util::stat_T metacache(this->path());
    bool valid = false;

    const util::string expire(optget("metacache.expire", util::string));
    const util::string lastsync(optget("localstatedir", util::string)+LASTSYNC);

    if (metacache.exists())
    {
        if (expire == "lastsync")
        {
            const util::string path(this->_portdir + "/metadata/timestamp");
            bool has_timestamp = util::is_file(path);
            bool has_lastsync  = util::is_file(lastsync);

            if (has_timestamp and has_lastsync)
            {
                util::file_T t(path), l(lastsync);

                if (optget("debug", bool))
                {
                    debug_msg("Checking timestamps...");
                    t.dump(std::cout);
                    l.dump(std::cout);
                }

                valid = (t == l);
                
                if (not valid)
                {
                    debug_msg("timestamps don't match ; replacing lastsync");
                    t.close(); l.close();
                    util::copy_file(path, lastsync);
                }
            }
            else if (has_lastsync)
            {
                unlink(LASTSYNC);
                valid = ((std::time(NULL) - metacache.mtime())
                        < METACACHE_EXPIRE);
            }
            else if (has_timestamp)
                util::copy_file(path, lastsync);
            else
                valid = ((std::time(NULL) - metacache.mtime())
                        < METACACHE_EXPIRE);
        }
        else
            valid = ((std::time(NULL) - metacache.mtime()) <
                    std::strtol(expire.c_str(), NULL, 10));

        /* only valid if size > 0 */
        if (valid)
            valid = (metacache.size() > 0);
    }

    /* 
     * only valid if first line is "version=VERSION".
     * this prevents weird bugs if for some reason the
     * metacache format changed between versions.
     *
     * also, if second line (which should be portdir=)
     * does not list the same portdir as our current
     * setting, it needs to be invalidated.
     */

    if (valid)
    {
        std::ifstream stream(this->path().c_str());
        if (not stream)
            throw util::bad_fileobject_E(this->path());

        std::string line;
        valid = (std::getline(stream, line) and
                (line == (std::string("version=")+VERSION)));

        if (valid)
            valid = (std::getline(stream, line) and
                    (line == (std::string("portdir=")+this->_portdir)));

        if (valid)
        {
            valid = (std::getline(stream, line) and
                    (line == (std::string("overlays=") +
                              util::join(this->_overlays, ':'))));
        }
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
        pkgcache_T::iterator p, e = pkgcache.end();
        for (p = pkgcache.begin() ; p != e ; ++p)
        {
            if (status)
                ++progress;

            util::path_T metadata(this->_portdir + "/" + (*p) + "/metadata.xml");
            if (metadata.exists())
            {
                /* parse it */
                const metadata_xml_T m(metadata);
                this->push_back(m.data(this->_portdir));
            }
        }
    }

    if (status)
        *(optget("outstream", std::ostream *)) << std::endl;
}

/*
 * Load cache from disk.  If a vector is given, only load lines
 * whose package is in the vector.
 */

void
metacache_T::load()
{
    if (not util::is_file(this->path()))
        return;

    try
    {
        const util::vars_T cache(this->path());

        this->_portdir = cache["portdir"];
        if (this->_portdir.empty())
            throw metacache_parse_E();

        /* reserve to prevent tons of reallocations */
        if (cache["size"].empty() or cache["size"] == "0")
            this->reserve(METACACHE_RESERVE);
        else
            this->reserve(std::atoi(cache["size"].c_str()));

        util::vars_T::const_iterator i, e = cache.end();
        for (i = cache.begin() ; i != e ; ++i)
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

            /* get herds */
            str = parts.front();
            parts.erase(parts.begin());
            meta.herds = str.split(',');

            /* get devs */
            if (not parts.empty())
            {
                str = parts.front();
                parts.erase(parts.begin());
                if (not str.empty())
                    meta.devs = str.split(',');
            }

            /* get longdesc */
            if (not parts.empty())
            {
                str = parts.front();
                parts.erase(parts.begin());

                /* longdesc contains a ':', so reconstruct it */
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
        std::cerr << "Error parsing " << this->path() << std::endl;
        throw;
    }
}

/*
 * Dump cache to disk.
 */

void
metacache_T::dump()
{
    std::ofstream f(this->path().c_str());
    if (not f)
        throw util::bad_fileobject_E(this->path());

    f << "version=" << VERSION << std::endl;
    f << "portdir=" << this->_portdir << std::endl;
    f << "overlays=" << util::join(this->_overlays, ':') << std::endl;
    f << "size=" << this->size() << std::endl;

    /* for each metadata_T object */
    iterator ci, ce;
    for (ci = this->begin(), ce = this->end()  ; ci != ce ; ++ci)
    {
        /*
         * format is the form of:
         *   cat/pkg=herd1,herd2:dev1,dev2:longdesc
         */

        util::string str;
        std::size_t n;

        f << ci->pkg << "=";

        /* herds */
        metadata_T::herds_type::iterator h, he = ci->herds.end();
        for (h = ci->herds.begin(), n = 1 ; h != he ; ++h, ++n)
        {
            str += (*h);
            if (n != ci->herds.size())
                str += ",";
        }

        f << str << ":";
        
        /* developers */
        metadata_T::herd_type::iterator d, de = ci->devs.end();
        for (d = ci->devs.begin(), n = 1, str.clear() ; d != de ; ++d, ++n)
        {
            str += d->first;
            if (n != ci->devs.size())
                str += ",";
        }

        f << str << ":";

        /* longdesc */
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
