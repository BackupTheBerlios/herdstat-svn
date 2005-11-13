/*
 * herdstat -- src/action/pkg.cc
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
#include <herdstat/util/functional.hh>
#include <herdstat/portage/metadata_xml.hh>

#include "action/meta.hh"
#include "action/pkg.hh"

using namespace herdstat;
using namespace gui;

PkgActionHandler::PkgActionHandler()
    : mcache(options.portdir()),
      status(not options.quiet() and not options.debug()),
      cache_is_valid(false), at_least_one_not_cached(false),
      herds_xml(GlobalHerdsXML())
{
}

const char * const
PkgActionHandler::id() const
{
    return "pkg";
}

const char * const
PkgActionHandler::desc() const
{
    return "Find packages maintained by the given herd/developer.";
}

const char * const
PkgActionHandler::usage() const
{
    return "pkg <herd(s)|developer(s)>";
}

Tab *
PkgActionHandler::createTab(WidgetFactory *widgetFactory)
{
    Tab *tab = widgetFactory->createTab();
    tab->set_title(this->id());

    return tab;
}

/*
 * Show search failure message.
 */

struct Error : std::binary_function<std::string, const util::Regex *, void>
{
    void operator()(const std::string& criteria, const util::Regex *with) const
    {
        Options& options(GlobalOptions());

        if (options.quiet())
            return;

        std::cerr
            << "Failed to find any packages maintained by '"
            << criteria << "'";

        if (with->empty())
        {
            std::cerr << "." << std::endl;
            return;
        }
    
        std::cerr << " with " << (options.dev() ? "herd":"developer") << " '"
            << (*with)() << "'." << std::endl;
    }
};

/*
 * Determine whether or not the metadata.xml matches our
 * search criteria.
 */

bool
PkgActionHandler::metadata_matches(const portage::metadata &meta,
                                   const std::string &criteria)
{
    const portage::Herds& herds(meta.herds());
    const portage::Developers& devs(meta.devs());

    if (options.dev())
    {
        if ((options.regex() and (devs.find(regexp) != devs.end()) and
            (with.empty() or (herds.find(with) != herds.end()) or
            (with() == "no-herd" and herds.empty()))) or
            (not options.regex() and (devs.find(criteria) != devs.end()) and
            (with.empty() or (herds.find(with) != herds.end()) or
            (with() == "no-herd" and herds.empty()))))
            return true;
    }
    else
    {
        if ((options.regex() and (herds.find(regexp) != herds.end())) or
            (not options.regex() and (herds.find(criteria) != herds.end())) or
            (criteria == "no-herd" and herds.empty()))
        {
            if (with.empty())
                return true;
            else
            {
                /* --no-maintainer was specified.  It's true if there
                 * are a) no maintainers, or b) the herd is listed as
                 * a maintainer. */
                if (with() == "none")
                {
                    if (devs.empty() or ((devs.size() == 1) and
                        (devs.find(criteria) != devs.end())))
                        return true;
                }
                else if (devs.find(with) != devs.end())
                    return true;
            }
        }
    }

    return false;
}

/*
 * Given a list of packages and a query object, parse their
 * metadata.xml's searching for ones that match the criteria.
 */

void
PkgActionHandler::search(const Query& query, pkgQuery &q)
{
    if (options.regex())
        regexp.assign(q.query);

    /* for each package in the vector */
    for (Query::const_iterator i = query.begin() ; i != query.end() ; ++i)
    {
        const std::string& pkg(i->second);

        /* parse it's metadata.xml */
        const std::string path(options.portdir() + "/" + pkg + "/metadata.xml");
        const portage::metadata_xml mxml(path, pkg);
        const portage::metadata& meta(mxml.data());

        /* does it match the criteria? */
        if (not metadata_matches(meta, q.query))
            continue;

        /* yep, so save it */
        q.insert(std::make_pair(meta.pkg(), meta.longdesc()));
    }

    /* cache it */
    qcache(q);
    /* save it in results map */
    matches.insert(std::make_pair(q.query, new pkgQuery(q)));
}

/*
 * Search metadata cache for the specified list of items.
 */

void
PkgActionHandler::search(const Query& query)
{
    /* for each metadata.xml */
    for (metacache::const_iterator m = mcache.begin() ; m != mcache.end() ; ++m)
    {
        /* for each specified herd/dev */
        for (Query::const_iterator i = query.begin() ; i != query.end() ; ++i)
        {
            const std::string& q(i->second);

            if (options.regex())
                regexp.assign(q);

            /* does it match the criteria? */
            if (metadata_matches(*m, q))
            {
                /* we've already inserted at least one package */
                std::map<std::string, pkgQuery * >::iterator mpos;
                if ((mpos = matches.find(q)) != matches.end())
                    mpos->second->insert(std::make_pair(m->pkg(),
                                                        m->longdesc()));
                /* nope, so create a new query object */
                else
                {
                    matches.insert(std::make_pair(q,
                                   new pkgQuery(q, with(), options.dev())));
                    matches[q]->date = std::time(NULL);
                    matches[q]->insert(std::make_pair(m->pkg(),
                                                       m->longdesc()));

                    if (options.dev())
                    {
                        matches[q]->info.set_user(q);
                        herds_xml.fill_developer(matches[q]->info);
                    }
                }
            }
            /* didn't match */
            else if (matches.find(q) == matches.end())
            {
                matches.insert(std::make_pair(q,
                               new pkgQuery(q, with(), options.dev())));
                matches[q]->date = std::time(NULL);

                if (options.dev())
                {
                    matches[q]->info.set_user(q);
                    herds_xml.fill_developer(matches[q]->info);
                }
            }
        }
    }

    /* cache everything */
    std::map<std::string, pkgQuery * >::iterator p;
    for (p = matches.begin() ; p != matches.end() ; ++p)
        qcache(*(p->second));
}

/*
 * Display the results of a single package query.
 */

void
PkgActionHandler::add_matches(pkgQuery *q, QueryResults * const results)
{
    assert(q);

    const portage::Herds& herds(herds_xml.herds());
    portage::Herds::const_iterator i;

    if (not options.quiet())
    {
        if (options.regex())
            results->add("Regex", q->query);
        else if (options.dev())
        {
            if (q->info.name().empty())
                results->add("Developer", q->query);
            else
                results->add("Developer",
                    q->info.name() + " (" + q->query + ")");

            results->add("Email", q->query + "@gentoo.org");
        }
        else if ((i = herds.find(q->query)) != herds.end())
        {
            results->add("Herd", q->query);
            if (not i->email().empty())
                results->add("Email", i->email());
            if (not i->desc().empty())
                results->add("Description", i->desc());
        }
        else
            results->add("Herd", q->query);

        if (q->empty())
            results->add("Packages(0)", "none");
        /* display first package on same line */
        else if (options.verbose() and options.color())
            results->add(util::sprintf("Packages(%d)", q->size()),
                color[blue] + q->begin()->first + color[none]);
        else
            results->add(util::sprintf("Packages(%d)", q->size()),
                q->begin()->first);
    }
    else if (not q->empty() and not options.count())
        results->add(q->begin()->first);

    /* display the category/package */
    pkgQuery::const_iterator p;
    pkgQuery::size_type pn = 1;
    for (p = ++(q->begin()) ; p != q->end() ; ++p, ++pn)
    {
        std::string longdesc;
            
        if (not p->second.empty())
            longdesc = util::tidy_whitespace(p->second);

        if ((options.verbose() and not options.quiet()) and
                not longdesc.empty())
        {
            if (results->size() > 1 and
                results->back() != QueryResults::value_type("", ""))
                results->add_linebreak();

            if (options.color())
                results->add(color[blue] + p->first + color[none]);
            else
                results->add(p->first);

            results->add(longdesc);
            debug_msg("longdesc(%s): '%s'", p->first.c_str(),
                longdesc.c_str());

            if (pn != q->size())
                results->add_linebreak();
        }
        else if (options.verbose() and not options.quiet())
        {
            if (options.color())
                results->add(color[blue] + p->first + color[none]);
            else
                results->add(p->first);
        }
        else if (not options.count())
            results->add(p->first);
    }
}

/*
 * Loop through results map, either displaying an error message
 * or calling display() for each query object.
 */

void
PkgActionHandler::add_matches(QueryResults * const results)
{
    std::map<std::string, pkgQuery * >::size_type n = 1;
    std::map<std::string, pkgQuery * >::iterator m;
    for (m = matches.begin() ; m != matches.end() ; ++m, ++n)
    {
        if (m->second->empty())
        {
            --n;
            if (matches.size() == 1 and options.iomethod() == "stream")
            {
                Error error;
                error(m->first, &with);
                this->do_cleanup(results);
                throw ActionException();
            }

            not_found.push_back(m->first);
            continue;
        }

        this->size() += m->second->size();

        /* was --metadata also specified? if so, construct the package
         * list.  When we're all done, the list will be passed to
         * action_meta_handler::operator(). */
        if (options.meta() and not options.count())
        {
            /* we're only interested in the package names */
            pkgQuery::iterator p;
            for (p = m->second->begin() ; p != m->second->end() ; ++p)
                packages.push_back(p->first);
        }
        else
        {
            add_matches(m->second, results);
            
            /* only skip a line if we're not on the last one */
            if (not options.count() and n != matches.size())
                if (not options.quiet()) //or (quiet and m->second->size() > 0))
                    results->add_linebreak();
        }
    }
}

void
PkgActionHandler::do_init(Query& query, QueryResults * const results)
{
    this->size() = 0;

    /* check PORTDIR */
    if (not util::is_dir(options.portdir()))
	throw FileException(options.portdir());

    /* setup with regex */
    with.assign(options.dev() ? options.with_herd() :
                                 options.with_dev(),
                                 util::Regex::icase);

    /* load previously cached results */
    if (options.querycache())
        qcache.load();

    /* search previously cached results for current queries
     * and insert into our matches map if found */
    if (not qcache.empty())
    {
        for (Query::iterator q = query.begin() ; q != query.end() ; )
        {
            std::string& str(q->second);

            /* does a previously cached query that
             * matches our criteria exist? */
            querycache::iterator qi =
                qcache.find(pkgQuery(str, with(), options.dev()));
            if (qi != qcache.end() and not qcache.is_expired(*qi))
            {
                debug_msg("found '%s' in query cache", str.c_str());

                matches.insert(std::make_pair(str, new pkgQuery(*qi)));
                matches[str]->query = str;
                matches[str]->with  = with();

                if (options.dev())
                {
                    matches[str]->info.set_user(str);
                    herds_xml.fill_developer(matches[str]->info);
                }

                q = query.erase(q);
            }
            /* is a wider-scoped query cached? */
            else if (qi == qcache.end() and not with.empty())
            {
                /* If so, use the results to narrow down what we need and
                 * partially load the metadata cache */
                qi = qcache.find(pkgQuery(str, "", options.dev()));
                if (qi != qcache.end() and not qcache.is_expired(*qi))
                {
                    /* 
                     * but only if there's less than a certain amount (since
                     * we're parsing the individual metadata.xml's).  It's only
                     * faster than metacache.load() + search(opts) up to a
                     * certain number. 
                     */

                    const opts_type pkgs(qi->pkgs());
                    if (pkgs.size() < 100)
                    {
                        pkgQuery pq(str, with(), options.dev());
                        pq.date = std::time(NULL);
                        pq.with = with();

                        if (options.dev())
                        {
                            pq.info.set_user(str);
                            herds_xml.fill_developer(pq.info);
                        }

                        Query pkgq;
                        std::transform(pkgs.begin(), pkgs.end(),
                            std::back_inserter(pkgq), util::EmptyFirst());
                        search(pkgq, pq);
                        q = query.erase(q);

                    } else ++q;
                } else ++q;
            } else ++q;
        }
    }

    if (options.debug())
    {
        debug_msg("query.size() after querycache search = %d", query.size());
        for (Query::iterator q = query.begin() ; q != query.end() ; ++q)
            std::cout << q->second << std::endl;
    }

    at_least_one_not_cached = (not query.empty());
    cache_is_valid = (options.metacache() and mcache.valid());

    if (cache_is_valid and at_least_one_not_cached)
        mcache.load();
    else if (at_least_one_not_cached)
    {
        /* fill cache and dump to disk */
        mcache.fill();
        mcache.dump();
    }

    debug_msg("mcache.size() == %d", mcache.size());
}

void
PkgActionHandler::do_regex(Query& query, QueryResults * const results)
{
}

void
PkgActionHandler::do_results(Query& query, QueryResults * const results)
{
    if (at_least_one_not_cached)
        search(query);

    add_matches(results);

    if (options.count())
    {
        results->add(util::sprintf("%d", this->size()));
        options.set_count(false); /* otherwise flush() will display size again */
    }
    else if (options.meta())
    {
        /* disable stuff we've handled already */
        const bool re(options.regex());
        const bool ere(options.eregex());
        const bool t(options.timer());

        options.set_regex(false);
        options.set_eregex(false);
        options.set_timer(false);

        /* get rid of any package dupes */
        std::sort(packages.begin(), packages.end());
        packages.erase(std::unique(packages.begin(), packages.end()),
            packages.end());

        Query q;
        std::transform(packages.begin(), packages.end(),
            std::back_inserter(q), util::EmptyFirst());

        /* ...and pass it to the metadata handler */
        MetaActionHandler mhandler;
        mhandler(q, results);

        options.set_regex(re);
        options.set_eregex(ere);
        options.set_timer(t);
    }

    if (not not_found.empty())
    {
        std::for_each(not_found.begin(), not_found.end(),
            std::bind2nd(Error(), &with));
    }

    if (options.querycache())
        qcache.dump();
}

void
PkgActionHandler::do_cleanup(QueryResults * const results)
{
    ActionHandler::do_cleanup(results);

    std::map<std::string, pkgQuery * >::iterator m;
    for (m = matches.begin() ; m != matches.end() ; ++m)
        delete m->second;
    matches.clear();
}

/* vim: set tw=80 sw=4 fdm=marker et : */
