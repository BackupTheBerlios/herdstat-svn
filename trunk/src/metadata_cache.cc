/*
 * herdstat -- src/metadata_cache.cc
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
#include <cstdlib>

#include <herdstat/xml/exceptions.hh>
#include <herdstat/util/string.hh>
#include <herdstat/util/vars.hh>
#include <herdstat/util/progress/percent.hh>
#include <herdstat/util/functional.hh>
#include <herdstat/io/binary_stream_iterator.hh>
#include <herdstat/portage/config.hh>
#include <herdstat/portage/metadata_xml.hh>

#include "common.hh"
#include "package_cache.hh"
#include "metadata_cache.hh"

#define METACACHE               /*LOCALSTATEDIR*/"/metacache"
#define METACACHE_EXPIRE        259200 /* 3 days */
#define METACACHE_DELIM         "%%%"

using namespace herdstat;
using namespace herdstat::portage;
using namespace herdstat::xml;

MetadataCache::MetadataCache()
    : Cache(GlobalOptions().localstatedir()+METACACHE),
      _spinner(NULL),
      _portdir(_options.portdir()),
      _overlays(_options.overlays())
{
}

MetadataCache::~MetadataCache() throw()
{
}

std::size_t
MetadataCache::cache_size() const
{
    return _metadatas.size();
}

const char * const
MetadataCache::name() const
{
    return "metadata";
}

/*
 * Is the cache valid?
 */

bool
MetadataCache::do_is_valid()
{
    BacktraceContext c("MetadataCache::is_valid()");

    const util::Stat mcache(this->path());
    bool valid = false;

    const std::string expire(_options.metacache_expire());
    const std::string lastsync(_options.localstatedir()+LASTSYNC);

    if (mcache.exists())
    {
        if (expire == "lastsync")
        {
            const std::string path(this->_portdir + "/metadata/timestamp");
            bool has_timestamp = util::is_file(path);
            bool has_lastsync  = util::is_file(lastsync);

            if (has_timestamp and has_lastsync)
            {
                util::File t(path), l(lastsync);

                if (_options.debug())
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
                valid = ((std::time(NULL) - mcache.mtime())
                        < METACACHE_EXPIRE);
            }
            else if (has_timestamp)
                util::copy_file(path, lastsync);
            else
                valid = ((std::time(NULL) - mcache.mtime())
                        < METACACHE_EXPIRE);
        }
        else
            valid = ((std::time(NULL) - mcache.mtime()) <
                    std::strtol(expire.c_str(), NULL, 10));

        /* only valid if size > 0 */
        if (valid)
            valid = (mcache.size() > 0);
    }

    return valid;
}

/*
 * Find and parse every metadata.xml in the tree,
 * filling our container with data.
 */

void
MetadataCache::do_fill()
{
    BacktraceContext c("MetadataCache::fill()");

    const bool status = (not _options.quiet() and not _options.debug());
        
    const PackageCache& pkgcache(GlobalPkgCache(_spinner));
    debug_msg("pkgcache.size() == %d", pkgcache.size());

    if (_spinner)
        _spinner->stop();

    util::PercentMeter percentage;
    if (status)
        percentage.start(pkgcache.size(), "Generating metadata.xml cache:");

    /* we will contain at most pkgcache.size() elements */
    _metadatas.reserve(pkgcache.size());

    const std::string base(_portdir+"/");

    /* for each pkg */
    PackageCache::const_iterator i, end;
    for (i = pkgcache.begin(), end = pkgcache.end() ; i != end ; ++i)
    {
        if (status)
            ++percentage;

        const std::string path(base+i->full()+"/metadata.xml");
        if (util::file_exists(path))
        {
            /* parse it */
            const MetadataXML meta(path, *i);
            _metadatas.push_back(meta.data());
        }
    }

    /* trim unused space */
    if (_metadatas.capacity() > (_metadatas.size() + 10))
        std::vector<Metadata>(_metadatas).swap(_metadatas);
}

/*
 * Load cache from disk.
 */

struct CacheEntryToMetadata
    : std::binary_function<std::string, util::ProgressMeter *, portage::Metadata>
{
    portage::Metadata
    operator()(const std::string& entry, util::ProgressMeter *progress) const
    {
        if (progress)
            ++*progress;

        std::vector<std::string> parts;
        util::split(entry, std::back_inserter(parts), METACACHE_DELIM, true);
        if (parts.size() != 4)
            throw ParserException(GlobalOptions().localstatedir()+METACACHE,
                                  "Invalid format: '"+entry+"'.");

        portage::Metadata meta(parts[0]);
        portage::Herds& herds(meta.herds());
        portage::Developers& devs(meta.devs());

        /* assign herds */
        util::split(parts[1], std::inserter(herds, herds.end()), ",");
        /* assign developers */
        util::split(parts[2], std::inserter(devs, devs.end()), ",");

        /* assign longdesc */
        meta.set_longdesc(parts[3]);

        return meta;
    }
};

struct MetadataToCacheEntry
{
    std::string
    operator()(const portage::Metadata& meta) const
    {
        /*
         * format is the form of:
         *   cat/pkg:herd1,herd2:dev1,dev2:longdesc
         */

        return (meta.pkg() + METACACHE_DELIM +
                util::join(meta.herds().begin(),
                           meta.herds().end(), ",") + METACACHE_DELIM +
                util::join(meta.devs().begin(),
                           meta.devs().end(), ",") + METACACHE_DELIM +
                util::tidy_whitespace(meta.longdesc()));
    }
};

void
MetadataCache::do_load(io::BinaryIStream& stream)
{
    BacktraceContext c("MetadataCache::load()");

    _metadatas.reserve(this->header_size());
    std::transform(io::BinaryIStreamIterator<std::string>(stream),
                   io::BinaryIStreamIterator<std::string>(),
                   std::back_inserter(_metadatas),
                   std::bind2nd(CacheEntryToMetadata(), _spinner));
}

/*
 * Dump cache to disk.
 */

void
MetadataCache::do_dump(io::BinaryOStream& stream)
{
    BacktraceContext c("MetadataCache::dump()");

    std::transform(_metadatas.begin(), _metadatas.end(),
        io::BinaryOStreamIterator<std::string>(stream),
        MetadataToCacheEntry());
}

/* vim: set tw=80 sw=4 fdm=marker et : */
