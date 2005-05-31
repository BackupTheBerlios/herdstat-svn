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

#include "metadata_xml.hh"
#include "metacache.hh"

#define METACACHE               LOCALSTATEDIR"/metacache"
#define METACACHE_EXPIRE        592200      /* one week */
#define METACACHE_RESERVE       8600
#define LASTSYNC                LOCALSTATEDIR"/lastsync"

/*
 * metadata_T
 */

bool
metadata_T::dev_exists(const herd_type::value_type &dev) const
{
    herd_type::value_type d(dev);
    if (dev.find('@') == herd_type::value_type::npos)
        d.append("@gentoo.org");
    return std::find(this->devs.begin(), this->devs.end(),
            d) != this->devs.end();
}

bool
metadata_T::dev_exists(const util::regex_T &regex) const
{
    herd_type::const_iterator i;
    for (i = this->devs.begin() ; i != this->devs.end() ; ++i)
        if (regex == *i)
            return true;
    return false;
}

bool
metadata_T::herd_exists(const herds_type::value_type &herd) const
{
    return std::find(this->herds.begin(), this->herds.end(),
            herd) != this->herds.end();
}

bool
metadata_T::herd_exists(const util::regex_T &regex) const
{
    herds_type::const_iterator i;
    for (i = this->herds.begin() ; i != this->herds.end() ; ++i)
        if (regex == *i)
            return true;
    return false;
}

void
metadata_T::dump(const std::ostream &stream)
{

}

void
metadata_T::get_pkg_from_path()
{
    assert(not this->path.empty());
    this->pkg = this->path.substr(this->_portdir.size() + 1);
    string_type::size_type pos = pkg.find("/metadata.xml");
    if (pos != string_type::npos)
        this->pkg = this->pkg.substr(0, pos);
}

/*
 * metacache_T
 */

metacache_T::metacache_T(const string_type &p)
    : util::cache_T<value_type>(METACACHE), _portdir(p)
{
    this->_cache.reserve(METACACHE_RESERVE);
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
    portage::categories_T::const_iterator c;
    for (c = categories.begin() ; c != categories.end() ; ++c)
    {
        const util::path_T path(this->_portdir + "/" + (*c));
        debug_msg("traversing %s...", path.c_str());

        if (status)
            ++progress;

        if (not util::is_dir(path))
            continue;

        const util::dir_T category(path);
        util::dir_T::const_iterator d;

        /* for each directory in this category */
        for (d = category.begin() ; d != category.end() ; ++d)
            if (util::is_file(*d + "/metadata.xml"))
                this->push_back(this->parse_metadata(*d + "/metadata.xml"));
    }

    if (timer)
    {
        elapsed.stop();
        debug_msg("Took %ldms to process %d metadata.xml's.",
            elapsed.elapsed(), this->size());
    }

//    if (status)
//    {
//        *(optget("outstream", std::ostream *)) << " (total "
//            << this->size() << ")" << std::endl;
//    }
}

/*
 * Parse the given metadata.xml, and return a new metadata_T object.
 */

metadata_T
metacache_T::parse_metadata(const util::path_T &path)
{
    const metadata_xml_T metadata(path);

    /* TODO: just move metadata_T to metadata_xml_handler.hh?
     * and then just do a straight assignment */
    metadata_T meta(this->_portdir, path);
    meta.herds = metadata.herds();
    meta.longdesc = metadata.longdesc();

    metadata_xml_T::herd_type::iterator i;
    for (i = metadata.devs().begin() ; i != metadata.devs().end() ; ++i)
        meta.devs.push_back(i->first);

    return meta;
}

/*
 * Load cache from disk.
 */

void
metacache_T::load()
{
    if (not util::is_file(METACACHE))
        return;

    util::vars_T cache(METACACHE);
    this->_portdir = cache["portdir"];

    try
    {
        util::vars_T::iterator i;
        for (i = cache.begin() ; i != cache.end() ; ++i)
        {
            /* not a category/package, so skip it */
            if (i->first.find('/') == util::string::npos)
                continue;

            metadata_T meta(this->_portdir,
                this->_portdir + "/" + i->first + "/metadata.xml");

            std::vector<util::string> parts = i->second.split(':');
            if (parts.empty() or parts.size() > 2)
                throw metacache_parse_E();

            meta.herds = parts.front().split(',');
            
            if (parts.size() > 1)
                meta.devs  = parts.back().split(',');

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
    /* TODO: open in binary mode??? */
    std::auto_ptr<std::ofstream> f(new std::ofstream(METACACHE));
    if (not (*f))
        throw util::bad_fileobject_E(METACACHE);

    *f << "# Automatically generated by " << PACKAGE << "-" << VERSION
        << std::endl;
    *f << "portdir=" << this->_portdir << std::endl;
    *f << "size=" << this->size() << std::endl;

    /* for each metadata_T object */
    for (iterator i = this->begin() ; i != this->end() ; ++i)
    {
        /*
         * format is the form of:
         *   cat/pkg=herd1,herd2:dev1,dev2
         */

        util::string herds_string;
        metadata_T::herds_type::iterator h;
        for (h = i->herds.begin() ; h != i->herds.end() ; ++h)
            herds_string += (*h) + ",";
        
        if (not herds_string.empty())
            herds_string.erase(herds_string.length() - 1);

        util::string devs_string;
        metadata_T::herd_type::iterator d;
        for (d = i->devs.begin() ; d != i->devs.end() ; ++d)
            devs_string += (*d) + ",";

        if (not devs_string.empty())
            devs_string.erase(devs_string.length() - 1);
        
        *f << i->pkg << "=" << herds_string << ":" << devs_string
            << std::endl;
    }
}

/* vim: set tw=80 sw=4 et : */
