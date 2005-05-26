/*
 * herdstat -- src/pkgcache.cc
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

#include <libxml++/libxml++.h>
#include "pkgcache_xml_handler.hh"
#include "pkgcache.hh"

#define PKGCACHE_EXPIRE     592200  /* one week */

void
pkgCache_T::read()
{
    xml_T<pkgCacheXMLHandler_T> pkgcache_xml(PKGCACHE);
    pkgCacheXMLHandler_T *handler = pkgcache_xml.handler();

    pkgCacheXMLHandler_T::value_type::iterator i;
    for (i = handler->queries.begin() ; i != handler->queries.end() ; ++i)
        this->push_back(*(i->second));
}

void
pkgCache_T::write() const
{
    try
    {
        xmlpp::Document doc;
        xmlpp::Element *root = doc.create_root_node("queries");

        const_iterator i;
        size_type n = 1;
        for (i = this->begin() ; i != this->end() ; ++i, ++n)
        {
            /* <query id="n"> */
            xmlpp::Element *query_node = root->add_child("query");
            query_node->set_attribute("id", util::sprintf("%d", n));

            /* <search> */
            xmlpp::Element *search_node = query_node->add_child("search");
            search_node->set_child_text(i->query);

            /* <date> */
            xmlpp::Element *date_node = query_node->add_child("date");
            date_node->set_child_text(util::sprintf("%lu",
                static_cast<unsigned long>(i->date)));

            /* <results> */
            xmlpp::Element *results_node = query_node->add_child("results");
        
            value_type::const_iterator p;
            for (p = i->begin() ; p != i->end() ; ++p)
            {
                /* <pkg> */
                xmlpp::Element *pkg_node = results_node->add_child("pkg");

                /* <name> */
                xmlpp::Element *pkg_name_node = pkg_node->add_child("name");
                pkg_name_node->set_child_text(p->first);

                /* <longdesc> */
                xmlpp::Element *pkg_longdesc_node = pkg_node->add_child("longdesc");
                pkg_longdesc_node->set_child_text(p->second);
            }
        }

        doc.write_to_file_formatted(PKGCACHE, "UTF-8");
    }
    catch (const xmlpp::exception &e)
    {
        throw XMLWriter_E(PKGCACHE, e.what());
    }
}

pkgCache_T::iterator
pkgCache_T::find (const util::string &query)
{
    iterator i;
    for (i = this->begin() ; i != this->end() ; ++i)
    {
        if (query == i->query)
            return i;
    }
    
    return this->end();
}

/*
 * Check if the query exists in the cache
 * and hasn't expired.
 */

bool
pkgCache_T::exists(const util::string &query)
{
    bool found = false;

    iterator i;
    for (i = this->begin() ; i != this->end() ; ++i)
    {
        if (query == i->query/* and TODO: time is less than 1 week */)
        {
            found = true;
            break;
        }
    }

    return found;
}

/* vim: set tw=80 sw=4 et : */
