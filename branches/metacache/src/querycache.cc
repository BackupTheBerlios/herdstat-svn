/*
 * herdstat -- src/querycache.cc
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

#include "common.hh"

#ifdef USE_LIBXMLPP
# include <libxml++/libxml++.h>
#else
# include <xmlwrapp/xmlwrapp.h>
#endif /* USE_LIBXMLPP */

#include "querycache_xml_handler.hh"
#include "querycache.hh"

/* max number of queries to cache.
 * TODO: good candidate for an configuration file
 * option if we ever need one. */
#define QUERYCACHE_MAX      1000

/* amount of time (in seconds) a cached query
 * object is considered valid */
#define QUERYCACHE_EXPIRE   84600

#define QUERYCACHE          LOCALSTATEDIR"/querycache.xml"

/*
 * Add a pkgQuery_T object to the cache, removing
 * an expired pkgQuery_T object if it exists.
 */

void
queryCache_T::operator() (const pkgQuery_T &q)
{
    /* remove old cached query if it exists */
    iterator i = this->find(q);
    if (i != this->end())
    {
        /* dont cache it if it hasn't expired and size is equal */
        if (not is_expired(*i) and (q.size() == i->size()))
            return;

        debug_msg("old query exists for '%s', so removing it",
            q.query.c_str());
        this->_cache.erase(i);
    }

    this->_cache.push_back(q);
}

/*
 * Load our cache from disk.
 */

void
queryCache_T::load()
{
    if (not util::is_file(QUERYCACHE))
        return;

    xml_T<queryCacheXMLHandler_T> querycache_xml;
    querycache_xml.parse(QUERYCACHE);

    queryCacheXMLHandler_T *handler = querycache_xml.handler();
    queryCacheXMLHandler_T::value_type::iterator i;
    for (i = handler->queries.begin() ; i != handler->queries.end() ; ++i)
        this->_cache.push_back(pkgQuery_T(*(*i)));
}

/*
 * Sort queries oldest to newest.
 */

static bool
is_greater(pkgQuery_T q1, pkgQuery_T q2)
{
    return q1.date < q2.date;
}

void
queryCache_T::sort_oldest_to_newest()
{
    /* sort by date */
    std::stable_sort(this->begin(), this->end(), is_greater);
}

/*
 * Clean out old queries until size() == QUERYCACHE_MAX
 */

void
queryCache_T::cleanse()
{
    debug_msg("this->size() > QUERYCACHE_MAX(%d), so trimming oldest queries.",
        QUERYCACHE_MAX);

    this->sort_oldest_to_newest();

    /* while > QUERYCACHE_MAX, erase the first (oldest) query */
    while (this->size() > QUERYCACHE_MAX)
        this->_cache.erase(this->begin());
}

void
queryCache_T::dump(std::ostream &stream)
{
    stream << "Package cache (size: " << this->size()
        << ")" << std::endl;

    for (iterator i = this->begin() ; i != this->end() ; ++i)
    {
        i->dump(stream);
        stream << std::endl;
    }
}

/*
 * Dump our cache to disk.
 */

void
queryCache_T::dump()
{
    /* trim if needed */
    if (this->size() > QUERYCACHE_MAX)
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
            pkgQuery_T q = *i;

#ifdef USE_LIBXMLPP
            /* <query id="n"> */
            xmlpp::Element *query_node = root->add_child("query");

            /* <criteria> */
            xmlpp::Element *criteria_node = query_node->add_child("criteria");

            /* <string> */
            xmlpp::Element *query_string_node = criteria_node->add_child("string");
            query_string_node->set_child_text(q.query);

            /* <with> */
            xmlpp::Element *with_string_node = criteria_node->add_child("with");
            with_string_node->set_child_text(q.with);

            /* <type> */
            xmlpp::Element *type_node = criteria_node->add_child("type");
            type_node->set_child_text(q.type == QUERYTYPE_DEV ? "dev":"herd");

            /* <date> */
            xmlpp::Element *date_node = query_node->add_child("date");
            date_node->set_child_text(util::sprintf("%lu",
                static_cast<unsigned long>(q.date)));

            /* <results> */
            xmlpp::Element *results_node = query_node->add_child("results");

#else

            xml::node::iterator it = root.insert(root.begin(), xml::node("query"));

            xml::node cri_node("criteria");
            cri_node.push_back(xml::node("string", q.query.c_str()));
            cri_node.push_back(xml::node("with", q.with.c_str()));
            cri_node.push_back(xml::node("type", q.type == QUERYTYPE_DEV? "dev":"herd"));
            it->push_back(cri_node);

            it = root.begin();
            it->push_back(xml::node("date", util::sprintf("%lu",
                static_cast<unsigned long>(q.date)).c_str()));

            xml::node results("results");

#endif /* USE_LIBXMLPP */

            /* for each package in query results */
            pkgQuery_T::const_iterator p;
            for (p = q.begin() ; p != q.end() ; ++p)
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
        doc.write_to_file_formatted(QUERYCACHE, "UTF-8");
#else /* USE_LIBXMLPP */
        doc.save_to_file(QUERYCACHE, 0);
#endif /* USE_LIBXMLPP */
    }
#ifdef USE_LIBXMLPP
    catch (const xmlpp::exception &e)
#else
    catch (const std::exception &e)
#endif /* USE_LIBXMLPP */
    {
        throw XMLWriter_E(QUERYCACHE, e.what());
    }

#ifdef HAVE_LIBZ
    this->compress();
#endif

}

/*
 * Has the specified pkgQuery_T object expired?
 */

bool
queryCache_T::is_expired(const pkgQuery_T &q) const
{
    return ((std::time(NULL) - q.date) > QUERYCACHE_EXPIRE);
}

/*
 * Find a pkgQuery_T object.
 */

//static bool
//isEqual(pkgQuery_T *q1, pkgQuery_T *q2)
//{
//    return (*q1 == *q2);
//}

queryCache_T::iterator
queryCache_T::find(const pkgQuery_T &q)
{
//    return std::find_if(this->begin(), this->end(),
//        std::bind2nd(std::ptr_fun(isEqual), &q));
    return std::find(this->begin(), this->end(), q);
}

/*
 * Return a vector of query names in the cache.
 */

std::vector<util::string>
queryCache_T::queries() const
{
    std::vector<util::string> v;
    for (const_iterator i = this->begin() ; i != this->end() ; ++i)
    {
        util::string s(i->query);
        if (not i->with.empty())
            s.append(util::string("/") + i->with);
        v.push_back(s);
    }
    return v;
}

/*
 * Tidy up.
 */

//queryCache_T::~queryCache_T()
//{
//    for (iterator i = this->begin() ; i != this->end() ; ++i)
//        delete *i;
//}

/* vim: set tw=80 sw=4 et : */
