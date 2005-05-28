/*
 * herdstat -- src/pkgcache.cc
 * $Id: pkgcache.cc 361 2005-05-27 17:14:10Z ka0ttic $
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

#ifdef USE_LIBXMLPP
# include <libxml++/libxml++.h>
#else
# include <xmlwrapp/xmlwrapp.h>
#endif /* USE_LIBXMLPP */

#ifdef DEBUG
#include "formatter.hh"
#endif /* DEBUG */

#include "pkgcache_xml_handler.hh"
#include "pkgcache.hh"

/* max number of queries to cache.
 * TODO: good candidate for an configuration file
 * option if we ever need one. */
#define PKGCACHE_MAX        1000

/* amount of time (in seconds) a cached query
 * object is considered valid */
#define PKGCACHE_EXPIRE     592200  /* one week */

#define PKGCACHE    LOCALSTATEDIR"/pkgcache.xml"

#ifdef HAVE_LIBZ
# define PKGCACHE_ZL    PKGCACHE".zl"
#endif

void
pkgQuery_T::dump(std::ostream &stream) const
{
    formatter_T out;
    out.set_maxlabel(16);
    out.set_maxdata(optget("maxcol", std::size_t) - out.maxlabel());
    out.set_attrs();
    
    out("Query string", this->query);
    out("Query with", this->with);
    out("Query type", (this->type == QUERYTYPE_DEV ? "dev":"herd"));
    out("Query date", util::format_date(this->date, "%s") + " ("
        + util::format_date(this->date) + ")");

    for (const_iterator p = this->begin() ; p != this->end() ; ++p)
        out("", p->first);

    out.flush(stream);
}

bool
pkgQuery_T::operator== (const pkgQuery_T &that) const
{
//    debug_msg("pkgQuery_T::operator==");
//    debug_msg("%s == %s ? %d", this->query.c_str(), that.query.c_str(),
//            (this->query == that.query));
//    debug_msg("%s == %s ? %d", this->with.c_str(), that.with.c_str(),
//            (this->with == that.with));
//    debug_msg("%d == %d ? %d", this->type, that.type,
//            (this->type == that.type));

    return ((this->query == that.query) and
            (this->with  == that.with) and
            (this->type  == that.type));
}

/*
 * For each package we contain, return a vector
 * of paths to each metadata.xml.
 */

const std::vector<util::string>
pkgQuery_T::make_list(const util::string &portdir) const
{
    std::vector<util::string> v;

    for (const_iterator i = this->begin() ; i != this->end() ; ++i)
        v.push_back(portdir + "/" + i->first + "/metadata.xml");

    return v;
}

/*
 * Add a pkgQuery_T object to the cache, removing
 * an expired pkgQuery_T object if it exists.
 */

void
pkgCache_T::operator() (pkgQuery_T *q)
{
    /* remove old cached query if it exists */
    iterator i = this->find(*q);
    if (i != this->end())
    {
        /* dont cache it if it hasn't expired and size is equal */
        if (not is_expired(*i) and (q->size() == (*i)->size()))
        {
            delete q;
            return;
        }

        debug_msg("old query exists for '%s', so removing it",
            q->query.c_str());
        delete *i;
        this->erase(i);
    }

    q->id = this->size() + 1;
    this->push_back(q);
}

void
pkgCache_T::compress()
{
#ifdef HAVE_LIBZ

    if (util::is_file(PKGCACHE))
    {
        /* save previously existing */
        if (util::is_file(PKGCACHE_ZL))
            util::move_file(PKGCACHE_ZL, PKGCACHE_ZL".bak");

        /* compress */
        util::zlib_T zlib;
        if (zlib.compress(PKGCACHE, PKGCACHE_ZL) == Z_OK)
        {
            unlink(PKGCACHE);
            unlink(PKGCACHE_ZL".bak");
        }
        else
            util::move_file(PKGCACHE_ZL".bak", PKGCACHE_ZL);
    }

#endif /* HAVE_LIBZ */
}

void
pkgCache_T::decompress()
{
#ifdef HAVE_LIBZ

    if (util::is_file(PKGCACHE_ZL))
    {
        if (util::is_file(PKGCACHE))
            util::move_file(PKGCACHE, PKGCACHE".bak");

        /* uncompress */
        util::zlib_T zlib;
        if (zlib.decompress(PKGCACHE_ZL, PKGCACHE) == Z_OK)
        {
            unlink(PKGCACHE_ZL);
            unlink(PKGCACHE".bak");
        }
        else
            util::move_file(PKGCACHE".bak", PKGCACHE);
    }

#endif /* HAVE_LIBZ */
}

/*
 * Load our cache from disk to memory.
 */

void
pkgCache_T::load()
{
#ifdef HAVE_LIBZ
    this->decompress();
#endif /* HAVE_LIBZ */

    if (not util::is_file(PKGCACHE))
        return;

    xml_T<pkgCacheXMLHandler_T> pkgcache_xml;
    pkgcache_xml.parse(PKGCACHE);

    pkgCacheXMLHandler_T *handler = pkgcache_xml.handler();
    pkgCacheXMLHandler_T::value_type::iterator i;
    for (i = handler->queries.begin() ; i != handler->queries.end() ; ++i)
        this->push_back(new pkgQuery_T(*(*i)));

#ifdef HAVE_LIBZ
    this->compress();
#endif /* HAVE_LIBZ */
}

/*
 * Clean out old queries until size() == PKGCACHE_MAX
 */

static bool
is_greater(pkgQuery_T *q1, pkgQuery_T *q2)
{
    return q1->date < q2->date;
}

void
pkgCache_T::cleanse()
{
    debug_msg("this->size() > PKGCACHE_MAX(%d), so trimming oldest queries.",
        PKGCACHE_MAX);

    /* sort by date */
    std::stable_sort(this->begin(), this->end(), is_greater);

    /* while > PKGCACHE_MAX, erase the first (oldest) query */
    while (this->size() > PKGCACHE_MAX)
        this->erase(this->begin());
}

/*
 * Dump our cache to disk.
 */

void
pkgCache_T::dump(std::ostream *stream)
{
    /* stream should only be non-NULL for debugging purposes */
    if (stream)
    {
        *stream << "Package cache (size: " << this->size()
            << ")" << std::endl;

        for (iterator i = this->begin() ; i != this->end() ; ++i)
        {
            (*i)->dump(*stream);
            *stream << std::endl;
        }

        return;
    }

    /* trim if needed */
    if (this->size() > PKGCACHE_MAX)
        this->cleanse();

    /* generate XML */
    try
    {
#ifdef USE_LIBXMLPP
        xmlpp::Document doc;
        xmlpp::Element *root = doc.create_root_node("queries");
#else
        xml::init init;
        xml::document doc("queries");
        xml::node &root = doc.get_root_node();
#endif /* USE_LIBXMLPP */

        /* for each query */
        iterator i;
        size_type n = 1;
        for (i = this->begin() ; i != this->end() ; ++i, ++n)
        {
            pkgQuery_T *q = *i;

#ifdef USE_LIBXMLPP
            /* <query id="n"> */
            xmlpp::Element *query_node = root->add_child("query");
            query_node->set_attribute("id", util::sprintf("%d", q->id));

            /* <criteria> */
            xmlpp::Element *criteria_node = query_node->add_child("criteria");

            /* <string> */
            xmlpp::Element *query_string_node = criteria_node->add_child("string");
            query_string_node->set_child_text(q->query);

            /* <with> */
            xmlpp::Element *with_string_node = criteria_node->add_child("with");
            with_string_node->set_child_text(q->with);

            /* <type> */
            xmlpp::Element *type_node = criteria_node->add_child("type");
            type_node->set_child_text(q->type == QUERYTYPE_DEV ? "dev":"herd");

            /* <date> */
            xmlpp::Element *date_node = query_node->add_child("date");
            date_node->set_child_text(util::sprintf("%lu",
                static_cast<unsigned long>(q->date)));

            /* <results> */
            xmlpp::Element *results_node = query_node->add_child("results");

#else

            xml::node::iterator it = root.insert(root.begin(), xml::node("query"));
            it->get_attributes().insert("id", util::sprintf("%d", q->id).c_str());

            xml::node cri_node("criteria");
            cri_node.push_back(xml::node("string", q->query.c_str()));
            cri_node.push_back(xml::node("with", q->with.c_str()));
            cri_node.push_back(xml::node("type", q->type == QUERYTYPE_DEV? "dev":"herd"));
            it->push_back(cri_node);

            it = root.begin();
            it->push_back(xml::node("date", util::sprintf("%lu",
                static_cast<unsigned long>(q->date)).c_str()));

            xml::node results("results");

#endif /* USE_LIBXMLPP */

            /* for each package in query results */
            pkgQuery_T::const_iterator p;
            for (p = q->begin() ; p != q->end() ; ++p)
            {

#ifdef USE_LIBXMLPP
                /* <pkg> */
                xmlpp::Element *pkg_node = results_node->add_child("pkg");

                /* <name> */
                xmlpp::Element *pkg_name_node = pkg_node->add_child("name");
                pkg_name_node->set_child_text(p->first);

                /* <longdesc> */
                if (not p->second.empty())
                {
                    xmlpp::Element *pkg_longdesc_node =
                        pkg_node->add_child("longdesc");
                    pkg_longdesc_node->set_child_text(p->second);
                }

#else

                xml::node pkg("pkg");
                pkg.push_back(xml::node("name", p->first.c_str()));
                
                if (not p->second.empty())
                    pkg.push_back(xml::node("longdesc", p->second.c_str()));

                results.push_back(pkg);

#endif /* USE_LIBXMLPP */
            }

#ifdef USE_XMLWRAPP
            it->push_back(results);
#endif /* USE_XMLWRAPP */

        }

#ifdef USE_LIBXMLPP
        doc.write_to_file(PKGCACHE, "UTF-8");
#else /* USE_LIBXMLPP */
        doc.save_to_file(PKGCACHE, 0);
#endif /* USE_LIBXMLPP */
    }
#ifdef USE_LIBXMLPP
    catch (const xmlpp::exception &e)
#else
    catch (const std::exception &e)
#endif /* USE_LIBXMLPP */
    {
        throw XMLWriter_E(PKGCACHE, e.what());
    }

#ifdef HAVE_LIBZ
    this->compress();
#endif

}

/*
 * Has the specified pkgQuery_T object expired?
 */

bool
pkgCache_T::is_expired(const pkgQuery_T &q) const
{
    return ((std::time(NULL) - q.date) > PKGCACHE_EXPIRE);
}

/*
 * Find a pkgQuery_T object.
 */

static bool
isEqual(pkgQuery_T *q1, pkgQuery_T *q2)
{
    return (*q1 == *q2);
}

pkgCache_T::iterator
pkgCache_T::find(const pkgQuery_T &q)
{
    return std::find_if(this->begin(), this->end(),
        std::bind2nd(std::ptr_fun(isEqual), &q));
}

/*
 * Tidy up.
 */

pkgCache_T::~pkgCache_T()
{
    for (iterator i = this->begin() ; i != this->end() ; ++i)
        delete *i;
}

/* vim: set tw=80 sw=4 et : */
