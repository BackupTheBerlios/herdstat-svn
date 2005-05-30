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
//    MetadataXMLHandler_T *handler = metadata.handler();

    /* TODO: just move metadata_T to metadata_xml_handler.hh?
     * and then just do a straight assignment */
    metadata_T meta(path);
    meta.herds = metadata.herds();
    meta.devs  = metadata.devs();
    meta.longdesc = metadata.longdesc();

    this->push_back(meta);
}

void
metacache_T::compress()
{
#ifdef HAVE_LIBZ

    if (util::is_file(METACACHE_UNCOMPRESSED))
    {
        if (util::is_file(METACACHE_COMPRESSED))
            util::move_file(METACACHE_COMPRESSED, METACACHE_COMPRESSED".bak");

        /* compress */
        util::zlib_T zlib;
        if (zlib.compress(METACACHE_UNCOMPRESSED, METACACHE_COMPRESSED) == Z_OK)
        {
            unlink(METACACHE_UNCOMPRESSED);
            unlink(METACACHE_COMPRESSED".bak");
        }
        else
            util::move_file(METACACHE_COMPRESSED".bak", METACACHE_COMPRESSED);
    }

#endif /* HAVE_LIBZ */
}

void
metacache_T::decompress()
{
#ifdef HAVE_LIBZ

    if (util::is_file(METACACHE_COMPRESSED))
    {
        if (util::is_file(METACACHE_UNCOMPRESSED))
            util::move_file(METACACHE_UNCOMPRESSED, METACACHE_UNCOMPRESSED".bak");

        /* decompress */
        util::zlib_T zlib;
        if (zlib.decompress(METACACHE_COMPRESSED, METACACHE_UNCOMPRESSED) == Z_OK)
        {
            unlink(METACACHE_COMPRESSED);
            unlink(METACACHE_UNCOMPRESSED".bak");
        }
        else
            util::move_file(METACACHE_UNCOMPRESSED".bak", METACACHE_UNCOMPRESSED);
    }

#endif /* HAVE_LIBZ */
}

void
metacache_T::load()
{
#ifdef HAVE_LIBZ
    this->decompress();
#endif /* HAVE_LIBZ */

    if (not util::is_file(METACACHE_UNCOMPRESSED))
        return;

    xml_T<metacacheXMLHandler_T> metacache_xml;
    metacache_xml.parse(METACACHE_UNCOMPRESSED);

    metacacheXMLHandler_T *handler = metacache_xml.handler();
    std::copy(handler->metadatas.begin(), handler->metadatas.end(),
        std::back_inserter(*this));

#ifdef HAVE_LIBZ
    this->compress();
#endif /* HAVE_LIBZ */
}

void
metacache_T::dump()
{
    try
    {
#ifdef USE_LIBXMLPP
        xmlpp::Document doc;
        xmlpp::Element *root = doc.create_root_node("metadatas");
#else /* USE_LIBXMLPP */
        xml::init init;
        xml::document doc("metadatas");
        xml::node &root = doc.get_root_node();
#endif /* USE_LIBXMLPP */

        size_type n = 1;
        for (iterator i = this->begin() ; i != this->end() ; ++i)
        {
            metadata_T meta = *i;

#ifdef USE_LIBXMLPP
            xmlpp::Element *meta_node = root->add_child("metadata");
            meta_node->set_attribute("name", meta.pkg);

            xmlpp::Element *toplevel = meta_node->add_child
                (meta.is_category? "catmetadata" : "pkgmetadata");
#else /* USE_LIBXMLPP */
            xml::node::iterator it = root.insert(root.begin(),
                xml::node("metadata"));
            it->get_attributes().insert("name", meta.pkg);

            xml::node toplevel(meta.is_category? "catmetadata" : "pkgmetadata");
#endif /* USE_LIBXMLPP */

            metadata_T::herds_type::iterator h;
            for (h = meta.herds.begin() ; h != meta.herds.end() ; ++h)
            {
#ifdef USE_LIBXMLPP
                xmlpp::Element *herd_node = toplevel->add_child("herd");
                herd_node->set_child_text(*h);
#else /* USE_LIBXMLPP */
                toplevel.push_back(xml::node("herd", *h));
#endif /* USE_LIBXMLPP */
            }

            metadata_T::herd_type::iterator d;
            for (d = meta.devs.begin() ; d != meta.devs.end() ; ++d)
            {
                string_type estr(i->first);
                if (estr.find('@') == string_type::npos)
                    estr.append("@gentoo.org");

#ifdef USE_LIBXMLPP
                xmlpp::Element *dev_node = toplevel->add_child("maintainer");

                xmlpp::Element *email = dev_node->add_child("email");
                email->set_child_text(estr);

                if (not i->second->name.empty())
                {
                    xmlpp::Element *name = dev_node->add_child("name");
                    name->set_child_text(i->second->name);
                }
#else /* USE_LIBXMLPP */
                xml::node devnode("maintainer");
                devnode.push_back("email", estr);
                
                if (not i->second->name.empty())
                    devnode.push_back("name", i->second->name);

                toplevel.push_back(devnode);
#endif /* USE_LIBXMLPP */
            }

            if (not meta.longdesc.empty())
            {
#ifdef USE_LIBXMLPP
                xmlpp::Element *longdesc = toplevel->add_child("longdesc");
                longdesc->set_child_text(meta.longdesc);
#else /* USE_LIBXMLPP */
                toplevel.push_back("longdesc", meta.longdesc);
#endif /* USE_LIBXMLPP */
            }

#ifdef USE_XMLWRAPP
            it->push_back(toplevel);
#endif /* USE_XMLWRAPP */
        }

#ifdef USE_LIBXMLPP
        doc.write_to_file(METACACHE_UNCOMPRESSED, "UTF-8");
#else /* USE_LIBXMLPP */
        doc.save_to_file(METACACHE_UNCOMPRESSED, 0);
#endif /* USE_LIBXMLPP */
    }
#ifdef USE_LIBXMLPP
    catch (const xmlpp::exception &e)
#else /* USE_LIBXMLPP */
    catch (const std::exception &e)
#endif /* USE_LIBXMLPP */
    {
        throw XMLWriter_E(METACACHE_UNCOMPRESSED, e.what());
    }

#ifdef HAVE_LIBZ
    this->compress();
#endif /* HAVE_LIBZ */
}

/* vim: set tw=80 sw=4 et : */
