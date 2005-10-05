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
#include <herdstat/util/functional.hh>
#include <herdstat/portage/config.hh>
#include <herdstat/portage/metadata_xml.hh>

#include "common.hh"
#include "pkgcache.hh"
#include "metacache.hh"

#define METACACHE               /*LOCALSTATEDIR*/"/metacache"
#define METACACHE_EXPIRE        259200 /* 3 days */
#define METACACHE_RESERVE       9100

using namespace herdstat;
using namespace herdstat::portage;

metacache_T::metacache_T(const std::string &portdir)
    : cachable(options::localstatedir()+METACACHE),
      _portdir(portdir),
      _overlays(options::overlays())
{
}

metacache_T::~metacache_T()
{
}

/*
 * Is the cache valid?
 */

bool
metacache_T::valid() const
{
    const util::Stat metacache(this->path());
    bool valid = false;

    const std::string expire(options::metacache_expire());
    const std::string lastsync(options::localstatedir()+LASTSYNC);

    if (metacache.exists())
    {
        if (expire == "lastsync")
        {
            const std::string path(this->_portdir + "/metadata/timestamp");
            bool has_timestamp = util::is_file(path);
            bool has_lastsync  = util::is_file(lastsync);

            if (has_timestamp and has_lastsync)
            {
                util::File t(path), l(lastsync);

                if (options::debug())
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
    const bool status = not options::quiet() and not options::debug();
    {
        util::Progress progress;
        pkgcache_T pkgcache(this->_portdir);
        debug_msg("pkgcache.size() == %d", pkgcache.size());

        if (status)
        {
            *(options::outstream())
                << "Generating metadata.xml cache: ";
            progress.start(pkgcache.size());
        }

        /* for each pkg */
        pkgcache_T::iterator i, end = pkgcache.end();
        for (i = pkgcache.begin() ; i != end ; ++i)
        {
            if (status)
                ++progress;

            char *path;
            asprintf(&path, "%s/%s/metadata.xml",
                _portdir.c_str(), i->c_str());

            if (util::file_exists(path))
            {
                /* parse it */
                const metadata_xml meta(path, *i);
                _metadatas.push_back(meta.data());
            }
        }
    }

    if (status)
        *(options::outstream()) << std::endl;
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
        const util::vars cache(this->path());

        /* reserve to prevent tons of reallocations */
        if (cache["size"].empty() or cache["size"] == "0")
            _metadatas.reserve(METACACHE_RESERVE);
        else
            _metadatas.reserve(util::destringify<int>(cache["size"]));

        util::vars::const_iterator i, e = cache.end();
        for (i = cache.begin() ; i != e ; ++i)
        {
            /* not a category/package, so skip it */
            if (i->first.find('/') == std::string::npos)
                continue;

            std::string str;
            metadata meta(i->first);
            Herds& herds(meta.herds());
            Developers&  devs(meta.devs());

            std::vector<std::string> parts = util::split(i->second, ':', true);
            if (parts.empty())
                throw Exception();

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

                meta.set_longdesc(str);
            }

            _metadatas.push_back(meta);
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

    std::string str;
    std::size_t n, size;

    /* for each metadata object */
    iterator ci, cend;
    for (ci = _metadatas.begin(), cend = _metadatas.end() ;
         ci != cend ; ++ci)
    {
        /*
         * format is the form of:
         *   cat/pkg=herd1,herd2:dev1,dev2:longdesc
         */

        f << ci->pkg() << "=";

        /* herds */
        {
            Herds::const_iterator i, end;
            for (i = ci->herds().begin(), end = ci->herds().end(),
                 n = 1, size = ci->herds().size(), str.clear() ;
                 i != end ; ++i, ++n)
            {
                str.append(i->name());
                if (n != size)
                    str.append(",");
            }
        }

        f << str << ":";
        
        /* developers */
        {
            Developers::const_iterator i, end;
            for (i = ci->devs().begin(), end = ci->devs().end(),
                 n = 1, size = ci->devs().size(), str.clear() ;
                 i != end ; ++i, ++n)
            {
                str.append(i->email());
                if (n != size)
                    str.append(",");
            }
        }

        /* longdesc */
        f << str << ":" << util::tidy_whitespace(ci->longdesc()) << std::endl;
    }
}

/* vim: set tw=80 sw=4 et : */
