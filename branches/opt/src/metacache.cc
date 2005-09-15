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

#include <iostream>
#include <algorithm>
#include <functional>
#include <herdstat/util/string.hh>
#include <herdstat/util/vars.hh>
#include <herdstat/util/progress.hh>
#include <herdstat/portage/config.hh>
#include <herdstat/portage/metadata_xml.hh>

#include "pkgcache.hh"
#include "metacache.hh"

#define METACACHE               /*LOCALSTATEDIR*/"/metacache"
#define METACACHE_EXPIRE        259200 /* 3 days */
#define METACACHE_RESERVE       9100

using namespace portage;

metacache_T::metacache_T(const std::string &portdir)
    : cachable(optget("localstatedir", std::string)+METACACHE),
      _portdir(portdir),
      _overlays(optget("portage.config", portage::config_T).overlays())
{
}

metacache_T::~metacache_T()
{
    std::for_each(this->begin(), this->end(), util::DeleteAndNullify());
}

/*
 * Is the cache valid?
 */

bool
metacache_T::valid() const
{
    const util::stat_T metacache(this->path());
    bool valid = false;

    const std::string expire(optget("metacache.expire", std::string));
    const std::string lastsync(optget("localstatedir", std::string)+LASTSYNC);

    if (metacache.exists())
    {
        if (expire == "lastsync")
        {
            const std::string path(this->_portdir + "/metadata/timestamp");
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
            throw FileException(this->path());

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

            const std::string path(this->_portdir + "/" + (*p) + "/metadata.xml");
            if (util::is_file(path))
            {
                /* parse it */
                const metadata_xml meta(path);
                metadata *data = new metadata(meta.data());
                data->set_pkg(*p);
                this->push_back(data);
            }
        }
    }

    if (status)
        *(optget("outstream", std::ostream *)) << std::endl;
}

/*
 * Load cache from disk.
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
            throw Exception();

        /* reserve to prevent tons of reallocations */
        if (cache["size"].empty() or cache["size"] == "0")
            this->reserve(METACACHE_RESERVE);
        else
            this->reserve(util::destringify<int>(cache["size"]));

        util::vars_T::const_iterator i, e = cache.end();
        for (i = cache.begin() ; i != e ; ++i)
        {
            /* not a category/package, so skip it */
            if (i->first.find('/') == std::string::npos)
                continue;

            std::string str;
            metadata *meta = new metadata(i->first);
            Herds& herds(meta->herds());
            Developers&  devs(meta->devs());

            std::vector<std::string> parts = util::split(i->second, ':', true);
            if (parts.empty())
            {
                delete meta;
                throw Exception();
            }

            /* get herds */
            str = parts.front();
            parts.erase(parts.begin());
            herds = util::split(str, ',');

            /* get devs */
            if (not parts.empty())
            {
                str = parts.front();
                parts.erase(parts.begin());
                if (not str.empty())
                    devs = util::split(str, ',');
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

                meta->set_longdesc(str);
            }

            this->push_back(meta);
        }
    }
    catch (const Exception)
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
        throw FileException(this->path());

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

        const Herds& herds((*ci)->herds());
        const Developers& devs((*ci)->devs());
        std::string str;
        std::size_t n;

        f << (*ci)->pkg() << "=";

        /* herds */
        {
            Herds::const_iterator i, end = herds.end();
            for (i = herds.begin(), n = 1 ; i != end ; ++i, ++n)
            {
                str += (*i)->name();
                if (n != herds.size())
                    str += ",";
            }
        }

        f << str << ":";
        
        /* developers */
        {
            Developers::const_iterator i, end = devs.end();
            for (i = devs.begin(), n = 1, str.clear() ; i != end ; ++i, ++n)
            {
                str += (*i)->email();
                if (n != devs.size())
                    str += ",";
            }
        }

        f << str << ":";

        /* longdesc */
        if ((*ci)->longdesc().empty())
            f << std::endl;
        else
            f << (*ci)->longdesc() << std::endl;
    }
}

/* vim: set tw=80 sw=4 et : */
