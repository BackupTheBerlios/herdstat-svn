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
#define METACACHE_EXPIRE        259200 /* 3 days */
#define METACACHE_RESERVE       8600
#define LASTSYNC                LOCALSTATEDIR"/lastsync"

/*
 * metadata_T
 */

bool
metadata_T::dev_exists(const string_type &dev) const
{
    string_type d(dev);
    if (dev.find('@') == string_type::npos)
        d.append("@gentoo.org");
    return std::find(this->devs.begin(), this->devs.end(),
            d) != this->devs.end();
}

bool
metadata_T::dev_exists(const util::regex_T &regex) const
{
    value_type::const_iterator i;
    for (i = this->devs.begin() ; i != this->devs.end() ; ++i)
        if (regex == *i)
            return true;
    return false;
}

bool
metadata_T::herd_exists(const string_type &herd) const
{
    return std::find(this->herds.begin(), this->herds.end(),
            herd) != this->herds.end();
}

bool
metadata_T::herd_exists(const util::regex_T &regex) const
{
    value_type::const_iterator i;
    for (i = this->herds.begin() ; i != this->herds.end() ; ++i)
        if (regex == *i)
            return true;
    return false;
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
    const bool status = not optget("quiet", bool) and not optget("debug", bool);
    {
        util::progress_T progress;
        const portage::categories_T categories(this->_portdir,
            optget("qa", bool));

        if (status)
        {
            *(optget("outstream", std::ostream *))
                << "Generating metadata.xml cache: ";
            progress.start(categories.size());
        }

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
            {
                util::path_T metadata(*d + "/metadata.xml");
                if (access(metadata.c_str(), F_OK) == 0)
                    this->push_back(this->parse(metadata));
            }
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
 * Load cache from disk.  If a vector is given, only load lines
 * whose package is in the vector.
 */

void
metacache_T::load(std::vector<util::string> v)
{
    if (not util::is_file(METACACHE))
        return;

    try
    {
        util::vars_T cache(METACACHE);
        this->_portdir = cache["portdir"];
        if (this->_portdir.empty())
            throw metacache_parse_E();

        if (not v.empty())
            this->reserve(v.size());
        else if (not cache["size"].empty())
            this->reserve(std::atoi(cache["size"].c_str()));
        else
            this->reserve(METACACHE_RESERVE);

        util::vars_T::iterator i;
        for (i = cache.begin() ; i != cache.end() ; ++i)
        {
            /* not a category/package, so skip it */
            if (i->first.find('/') == util::string::npos)
                continue;

            /* vector specified and not in vector, so skip it */
            if (not v.empty() and
                std::find(v.begin(), v.end(), i->first) == v.end())
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
        metadata_T::value_type::iterator mi;

        f << ci->pkg << "=";

        for (mi = ci->herds.begin(), n = 1 ; mi != ci->herds.end() ; ++mi, ++n)
        {
            str += (*mi);
            if (n != ci->herds.size())
                str += ",";
        }

        f << str << ":";
        
        for (mi = ci->devs.begin(), n = 1, str.clear() ; mi != ci->devs.end() ; 
            ++mi, ++n)
        {
            str += (*mi);
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
