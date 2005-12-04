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
#include <xmlwrapp/xmlwrapp.h>

#include <herdstat/util/file.hh>
#include <herdstat/util/string.hh>
#include <herdstat/xml/saxparser.hh>
#include <herdstat/xml/document.hh>

#include "common.hh"
#include "querycache.hh"

using namespace herdstat;
using namespace herdstat::xml;

/*
 * Content Handler for our internal querycache.xml
 */

class querycacheXMLHandler : public herdstat::xml::SAXHandler
{
    public:
        querycacheXMLHandler();
        virtual ~querycacheXMLHandler();

        std::vector<pkgQuery> queries;

    protected:
        /* callbacks */
        virtual bool start_element(const std::string&, const attrs_type&);
        virtual bool end_element(const std::string&);
        virtual bool do_text(const std::string&);

    private:
        /* internal state variables */
        bool in_query, in_string, in_with, in_type, in_results, in_pkg,
             in_portdir, in_overlays;
        std::string cur_pkg, cur_date;
        pkgQuery::iterator cur_query;
};

querycacheXMLHandler::querycacheXMLHandler()
    : in_query(false), in_string(false), in_with(false), in_type(false),
      in_results(false), in_pkg(false), in_portdir(false), in_overlays(false),
      cur_pkg(), cur_date(), cur_query()
{
}

querycacheXMLHandler::~querycacheXMLHandler()
{
}

bool
querycacheXMLHandler::start_element(const std::string &name,
                                    const attrs_type &attrs)
{
    if (name == "query")
    {
        in_query = true;

        attrs_type::const_iterator pos = attrs.find("date");
        if (pos != attrs.end())
            cur_date.assign(pos->second);
    }
    else if (name == "string" and in_query)
        in_string = true;
    else if (name == "with" and in_query)
        in_with = true;
    else if (name == "type" and in_query)
        in_type = true;
    else if (name == "portdir" and in_query)
        in_portdir = true;
    else if (name == "overlays" and in_query)
        in_overlays = true;
    else if (name == "results" and in_query)
        in_results = true;
    else if (name == "pkg" and in_results)
    {
        in_pkg = true;

        attrs_type::const_iterator pos = attrs.find("name");
        assert(pos != attrs.end());
        cur_pkg.assign(pos->second);
        cur_query = queries.back().insert(std::make_pair(cur_pkg, "")).first;
    }

    return true;
}

bool
querycacheXMLHandler::end_element(const std::string &name)
{
    if (name == "query")
        in_query = false;
    else if (name == "string")
        in_string = false;
    else if (name == "with")
        in_with = false;
    else if (name == "type")
        in_type = false;
    else if (name == "portdir")
        in_portdir = false;
    else if (name == "overlays")
        in_overlays = false;
    else if (name == "results")
        in_results = false;
    else if (name == "pkg")
        in_pkg = false;

    return true;
}

bool
querycacheXMLHandler::do_text(const std::string &text)
{
    if (in_string)
    {
        queries.push_back(pkgQuery(text));
        queries.back().date = util::destringify<long>(cur_date);
    }
    else if (in_with)
        queries.back().with = text;
    else if (in_type)
        queries.back().type =
            (text == "dev" ? QUERYTYPE_DEV : QUERYTYPE_HERD);
    else if (in_portdir)
        queries.back().portdir = text;
    else if (in_overlays)
        queries.back().overlays = util::split(text, ':');
    else if (in_pkg)
        cur_query->second += text;

    return true;
}

/****************************************************************************/

querycache::querycache()
    : _max(GlobalOptions().querycache_max()),
      _expire(GlobalOptions().querycache_expire()),
      _path(GlobalOptions().localstatedir()+"/querycache.xml")
{
}

/*
 * Add a pkgQuery object to the cache, removing
 * an expired pkgQuery object if it exists.
 */

void
querycache::operator() (const pkgQuery &q)
{
    /* remove old cached query if it exists */
    iterator i = this->find(q);
    if (i != this->end())
    {
        /* dont cache it if it hasn't expired and size is equal */
        if (not is_expired(*i) and (q.size() == i->size()))
            return;

        debug_msg("old query exists for '%s', so removing it", q.query.c_str());
        this->_queries.erase(i);
    }

    this->_queries.push_back(q);
}

/*
 * Load our cache from disk.
 */

void
querycache::load()
{
    if (not util::is_file(this->_path))
        return;

    Document<querycacheXMLHandler> querycache_xml;
    querycache_xml.parse(this->_path);
    querycacheXMLHandler *handler = querycache_xml.handler();

    _queries.insert(_queries.end(),
            handler->queries.begin(),
            handler->queries.end());
}

/*
 * Sort queries oldest to newest.
 */

struct LessDate
{
    bool operator()(const pkgQuery &q1, const pkgQuery &q2) const
    { return (q1.date < q2.date); }
};

void
querycache::sort()
{
    /* sort by date */
    std::stable_sort(_queries.begin(), _queries.end(), LessDate());
}

/*
 * Clean out old queries until size() == querycache_MAX
 */

void
querycache::purge_old()
{
    debug_msg("this->size() > querycache::_max(%d), so trimming oldest queries.",
        this->_max);

    /* while > querycache_MAX, erase the first (oldest) query */
    while (_queries.size() > static_cast<size_type>(this->_max))
        _queries.erase(_queries.begin());
}

void
querycache::dump(std::ostream &stream)
{
    stream << "Query cache (size: " << _queries.size()
        << "/" << this->_max << ")" << std::endl << std::endl;

    for (iterator i = _queries.begin() ; i != _queries.end() ; ++i)
    {
        i->dump(stream);
        stream << std::endl;
    }
}

/*
 * Dump our cache to disk.
 */

void
querycache::dump()
{
    BacktraceContext c("querycache::dump()");

    this->sort();

    /* trim if needed */
    if (_queries.size() > static_cast<size_type>(this->_max))
        this->purge_old();

    /* generate XML */
    try
    {
        ::xml::document doc("queries");
        doc.set_encoding("UTF-8");

        ::xml::node &root = doc.get_root_node();
        root.get_attributes().insert("total",
            util::sprintf("%d", _queries.size()).c_str());

        /* for each query */
        iterator i, e;
        for (i = _queries.begin(), e = _queries.end() ; i != e ; ++i)
        {
            ::xml::node query("query");
            query.get_attributes().insert("date",
                util::sprintf("%lu", static_cast<unsigned long>(i->date)).c_str());

            ::xml::node::iterator it = root.insert(root.begin(), query);

            ::xml::node cri_node("criteria");
            cri_node.push_back(::xml::node("string", i->query.c_str()));
            cri_node.push_back(::xml::node("with", i->with.c_str()));
            cri_node.push_back(::xml::node("type", i->type == QUERYTYPE_DEV? "dev":"herd"));
            cri_node.push_back(::xml::node("portdir", i->portdir.c_str()));
            cri_node.push_back(::xml::node("overlays",
                util::join(i->overlays, ':').c_str()));
            it->push_back(cri_node);

            it = root.begin();
            ::xml::node results("results");
            results.get_attributes().insert("total",
                util::sprintf("%d", i->size()).c_str());

            /* for each package in query results */
            pkgQuery::const_iterator pi, pe;
            for (pi = i->begin(), pe = i->end() ; pi != pe ; ++pi)
            {
                const std::string longdesc(util::tidy_whitespace(pi->second));
                ::xml::node pkg("pkg", longdesc.c_str());
                pkg.get_attributes().insert("name", pi->first.c_str());
                results.push_back(pkg);
            }

            it->push_back(results);
        }

        doc.save_to_file(this->_path.c_str(), 0);
    }
    catch (const std::exception &e)
    {
        throw Exception("Failed to write xml file: %s", this->_path.c_str());
    }
}

/*
 * Return a vector of query names in the cache.
 */

std::vector<std::string>
querycache::queries() const
{
    std::vector<std::string> v;
    for (const_iterator i = this->begin() ; i != this->end() ; ++i)
    {
        std::string s(i->query);
        if (not i->with.empty())
            s.append(std::string("/") + i->with);
        v.push_back(s);
    }
    return v;
}

/* vim: set tw=80 sw=4 fdm=marker et : */
