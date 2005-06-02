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
#define QUERYCACHE_MAX      100

/* amount of time (in seconds) a cached query
 * object is considered valid */
#define QUERYCACHE_EXPIRE   84600

#define QUERYCACHE          LOCALSTATEDIR"/querycache.xml"

/*
 * Add a pkgQuery_T object to the cache, removing
 * an expired pkgQuery_T object if it exists.
 */

void
querycache_T::operator() (const pkgQuery_T &q)
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
        this->erase(i);
    }

    this->push_back(q);
}

/*
 * Load our cache from disk.
 */

void
querycache_T::load()
{
    if (not util::is_file(QUERYCACHE))
        return;

    xml_T<querycacheXMLHandler_T> querycache_xml;
    querycache_xml.parse(QUERYCACHE);
    querycacheXMLHandler_T *handler = querycache_xml.handler();

    std::copy(handler->queries.begin(), handler->queries.end(),
        std::back_inserter(*this));
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
querycache_T::sort_oldest_to_newest()
{
    /* sort by date */
    std::stable_sort(this->begin(), this->end(), is_greater);
}

/*
 * Clean out old queries until size() == querycache_MAX
 */

void
querycache_T::purge_old()
{
    debug_msg("this->size() > querycache_MAX(%d), so trimming oldest queries.",
        QUERYCACHE_MAX);

    this->sort_oldest_to_newest();

    /* while > querycache_MAX, erase the first (oldest) query */
    while (this->size() > QUERYCACHE_MAX)
        this->erase(this->begin());
}

void
querycache_T::dump(std::ostream &stream)
{
    stream << "Query cache (size: " << this->size()
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
querycache_T::dump()
{
    /* trim if needed */
    if (this->size() > QUERYCACHE_MAX)
        this->purge_old();

    /* generate XML */
    try
    {
#ifdef USE_LIBXMLPP
        xmlpp::Document doc;
        xmlpp::Element *root = doc.create_root_node("queries");
        root->set_attribute("total", util::sprintf("%d", this->size()));
#else
        xml::init init;
        xml::document doc("queries");
        doc.set_encoding("UTF-8");

        xml::node &root = doc.get_root_node();
        root.get_attributes().insert("total",
            util::sprintf("%d", this->size()).c_str());
#endif /* USE_LIBXMLPP */

        /* for each query */
        iterator i;
        size_type n = 1;
        for (i = this->begin() ; i != this->end() ; ++i, ++n)
        {
#ifdef USE_LIBXMLPP
            /* <query id="n"> */
            xmlpp::Element *query_node = root->add_child("query");
            query_node->set_attribute("date",
                util::sprintf("%lu", static_cast<unsigned long>(i->date)));

            /* <criteria> */
            xmlpp::Element *criteria_node = query_node->add_child("criteria");

            /* <string> */
            xmlpp::Element *query_string_node = criteria_node->add_child("string");
            query_string_node->set_child_text(i->query);

            /* <with> */
            xmlpp::Element *with_string_node = criteria_node->add_child("with");
            with_string_node->set_child_text(i->with);

            /* <type> */
            xmlpp::Element *type_node = criteria_node->add_child("type");
            type_node->set_child_text(i->type == QUERYTYPE_DEV ? "dev":"herd");

            /* <results> */
            xmlpp::Element *results_node = query_node->add_child("results");
            results_node->set_attribute("total",
                util::sprintf("%d", i->size()));

#else

            xml::node query("query");
            query.get_attributes().insert("date",
                util::sprintf("%lu", static_cast<unsigned long>(i->date)).c_str());

            xml::node::iterator it = root.insert(root.begin(), query);

            xml::node cri_node("criteria");
            cri_node.push_back(xml::node("string", i->query.c_str()));
            cri_node.push_back(xml::node("with", i->with.c_str()));
            cri_node.push_back(xml::node("type", i->type == QUERYTYPE_DEV? "dev":"herd"));
            it->push_back(cri_node);

            it = root.begin();
            xml::node results("results");
            results.get_attributes().insert("total",
                util::sprintf("%d", i->size()).c_str());

#endif /* USE_LIBXMLPP */

            /* for each package in query results */
            pkgQuery_T::const_iterator p;
            for (p = i->begin() ; p != i->end() ; ++p)
            {

#ifdef USE_LIBXMLPP

                /* <pkg> */
                xmlpp::Element *pkg_node = results_node->add_child("pkg");
                pkg_node->set_attribute("name", p->first);
                pkg_node->set_child_text(p->second);

#else

                xml::node pkg("pkg", p->second.c_str());
                pkg.get_attributes().insert("name", p->first.c_str());
                results.push_back(pkg);

#endif /* USE_LIBXMLPP */
            }

#ifdef USE_XMLWRAPP
            it->push_back(results);
#endif /* USE_XMLWRAPP */

        }

#ifdef USE_LIBXMLPP
        doc.write_to_file(QUERYCACHE, "UTF-8");
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
}

/*
 * Has the specified pkgQuery_T object expired?
 */

bool
querycache_T::is_expired(const pkgQuery_T &q) const
{
    return ((std::time(NULL) - q.date) > QUERYCACHE_EXPIRE);
}

/*
 * Find a pkgQuery_T object.
 */

querycache_T::iterator
querycache_T::find(const pkgQuery_T &q)
{
    return std::find(this->begin(), this->end(), q);
}

/*
 * Return a vector of query names in the cache.
 */

std::vector<util::string>
querycache_T::queries() const
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

/* vim: set tw=80 sw=4 et : */
