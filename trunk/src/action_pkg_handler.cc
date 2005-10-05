/*
 * herdstat -- src/action_pkg_handler.cc
 * $Id$
 * Copyright (c) 2005 Aaron Walker <ka0ttic at gentoo.org>
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
#include <iomanip>
#include <algorithm>
#include <functional>
#include <iterator>
#include <map>
#include <herdstat/util/string.hh>
#include <herdstat/portage/metadata_xml.hh>
#include "action_meta_handler.hh"
#include "action_pkg_handler.hh"

using namespace herdstat;
using namespace herdstat::portage;
using namespace herdstat::util;

action_pkg_handler_T::action_pkg_handler_T()
    : action_herds_xml_handler_T(), metacache(options::portdir()),
      elapsed(0),
      status(not options::quiet() and not options::debug()),
      cache_is_valid(false), at_least_one_not_cached(false)
{
}

action_pkg_handler_T::~action_pkg_handler_T()
{
}

/*
 * Show search failure message.
 */

struct Error : std::binary_function<std::string, const util::Regex *, void>
{
    void operator()(const std::string& criteria, const util::Regex *with) const
    {
        if (options::quiet())
            return;

        std::cerr
            << "Failed to find any packages maintained by '"
            << criteria << "'";

        if (with->empty())
        {
            std::cerr << "." << std::endl;
            return;
        }
    
        std::cerr << " with " << (options::dev() ? "herd":"developer") << " '"
            << (*with)() << "'." << std::endl;
    }
};

/*
 * Determine whether or not the metadata.xml matches our
 * search criteria.
 */

bool
action_pkg_handler_T::metadata_matches(const metadata &meta,
                                       const std::string &criteria)
{
    const Herds& herds(meta.herds());
    const Developers& devs(meta.devs());

    if (options::dev())
    {
        if ((options::regex() and (devs.find(regexp) != devs.end()) and
            (with.empty() or (herds.find(with) != herds.end()) or
            (with() == "no-herd" and herds.empty()))) or
            (not options::regex() and (devs.find(criteria) != devs.end()) and
            (with.empty() or (herds.find(with) != herds.end()) or
            (with() == "no-herd" and herds.empty()))))
            return true;
    }
    else
    {
        if ((options::regex() and (herds.find(regexp) != herds.end())) or
            (not options::regex() and (herds.find(criteria) != herds.end())) or
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
action_pkg_handler_T::search(const opts_type &pkgs, pkgQuery_T &q)
{
    if (options::regex())
        regexp.assign(q.query);

    /* for each package in the vector */
    for (opts_type::const_iterator i = pkgs.begin() ; i != pkgs.end() ; ++i)
    {
        /* parse it's metadata.xml */
        const std::string path(options::portdir() + "/" + *i + "/metadata.xml");
        const metadata_xml mxml(path, *i);
        const metadata& meta(mxml.data());

        /* does it match the criteria? */
        if (not metadata_matches(meta, q.query))
            continue;

        /* yep, so save it */
        q.insert(std::make_pair(meta.pkg(), meta.longdesc()));
    }

    /* cache it */
    querycache(q);
    /* save it in results map */
    matches.insert(std::make_pair(q.query, new pkgQuery_T(q)));
}

/*
 * Search metadata cache for the specified list of items.
 */

void
action_pkg_handler_T::search(const opts_type &opts)
{
    /* for each metadata.xml */
    for (metacache_T::const_iterator m = metacache.begin() ;
         m != metacache.end() ; ++m)
    {
        /* for each specified herd/dev */
        for (opts_type::const_iterator i = opts.begin() ; i != opts.end() ; ++i)
        {
            if (options::regex())
                regexp.assign(*i);

            /* does it match the criteria? */
            if (metadata_matches(*m, *i))
            {
                /* we've already inserted at least one package */
                std::map<std::string, pkgQuery_T * >::iterator mpos;
                if ((mpos = matches.find(*i)) != matches.end())
                    mpos->second->insert(std::make_pair(m->pkg(),
                                                        m->longdesc()));
                /* nope, so create a new query object */
                else
                {
                    matches.insert(std::make_pair(*i,
                                   new pkgQuery_T(*i, with(), options::dev())));
                    matches[*i]->date = std::time(NULL);
                    matches[*i]->insert(std::make_pair(m->pkg(),
                                                       m->longdesc()));

                    if (options::dev())
                    {
                        matches[*i]->info.set_user(*i);
                        herdsxml.fill_developer(matches[*i]->info);
                    }
                }
            }
            /* didn't match */
            else if (matches.find(*i) == matches.end())
            {
                matches.insert(std::make_pair(*i,
                               new pkgQuery_T(*i, with(), options::dev())));
                matches[*i]->date = std::time(NULL);

                if (options::dev())
                {
                    matches[*i]->info.set_user(*i);
                    herdsxml.fill_developer(matches[*i]->info);
                }
            }
        }
    }

    /* cache everything */
    std::map<std::string, pkgQuery_T * >::iterator p;
    for (p = matches.begin() ; p != matches.end() ; ++p)
        querycache(*(p->second));
}

/*
 * Display the results of a single package query.
 */

void
action_pkg_handler_T::display(pkgQuery_T *q)
{
    assert(q);

    const Herds& herds(herdsxml.herds());
    Herds::const_iterator i;

    if (not options::quiet())
    {
        if (options::regex())
            output("Regex", q->query);
        else if (options::dev())
        {
            if (q->info.name().empty())
                output("Developer", q->query);
            else
                output("Developer",
                    q->info.name() + " (" + q->query + ")");

            output("Email", q->query + "@gentoo.org");
        }
        else if ((i = herds.find(q->query)) != herds.end())
        {
            output("Herd", q->query);
            if (not i->email().empty())
                output("Email", i->email());
            if (not i->desc().empty())
                output("Description", i->desc());
        }
        else
            output("Herd", q->query);


        if (q->empty())
            output("Packages(0)", "none");
        /* display first package on same line */
        else if (options::verbose() and options::color())
            output(util::sprintf("Packages(%d)", q->size()),
                color[blue] + q->begin()->first + color[none]);
        else
            output(util::sprintf("Packages(%d)", q->size()),
                q->begin()->first);
    }
    else if (not q->empty() and not options::count())
        output("", q->begin()->first);

    /* display the category/package */
    pkgQuery_T::const_iterator p;
    pkgQuery_T::size_type pn = 1;
    for (p = ++(q->begin()) ; p != q->end() ; ++p, ++pn)
    {
        std::string longdesc;
            
        if (not p->second.empty())
            longdesc = util::tidy_whitespace(p->second);

        if ((options::verbose() and not options::quiet()) and
                not longdesc.empty())
        {
            if (output.size() > 1 and output.peek() != "")
                output.endl();

            if (options::color())
                output("", color[blue] + p->first + color[none]);
            else
                output("", p->first);

            output("", longdesc);
            debug_msg("longdesc(%s): '%s'", p->first.c_str(),
                longdesc.c_str());

            if (pn != q->size())
                output.endl();
        }
        else if (options::verbose() and not options::quiet())
        {
            if (options::color())
                output("", color[blue] + p->first + color[none]);
            else
                output("", p->first);
        }
        else if (not options::count())
            output("", p->first);
    }
}

/*
 * Loop through results map, either displaying an error message
 * or calling display() for each query object.
 */

void
action_pkg_handler_T::display()
{
    /* set format attributes */
    if (not options::meta())
    {
        output.set_maxlabel(16);
        output.set_maxdata(options::maxcol() - output.maxlabel());
        if (options::devaway())
            output.set_devaway(devaway.keys());
        output.set_attrs();
    }

    opts_type::size_type n = 1;
    std::map<std::string, pkgQuery_T * >::iterator m;
    for (m = matches.begin() ; m != matches.end() ; ++m, ++n)
    {
        if (m->second->empty())
        {
            --n;
            if (matches.size() == 1)
            {
                Error error;
                error(m->first, &with);
                cleanup();
                throw ActionException();
            }

            not_found.push_back(m->first);
            continue;
        }

        size += m->second->size();

        /* was --metadata also specified? if so, construct the package
         * list.  When we're all done, the list will be passed to
         * action_meta_handler_T::operator(). */
        if (options::meta() and not options::count())
        {
            /* we're only interested in the package names */
            pkgQuery_T::iterator p;
            for (p = m->second->begin() ; p != m->second->end() ; ++p)
                packages.push_back(p->first);
        }
        else
        {
            display(m->second);
            
            /* only skip a line if we're not on the last one */
            if (not options::count() and n != matches.size())
                if (not options::quiet()) //or (quiet and m->second->size() > 0))
                    output.endl();
        }
    }
}

/*
 * Given a list of herds/devs, determine all packages belonging
 * to each herd/dev. For reliability reasons, every metadata.xml
 * in the tree is parsed.
 */

int
action_pkg_handler_T::operator() (opts_type &opts)
{
    /* action_pkg_handler doesn't support the all target */
    if (options::all())
    {
        std::cerr << "Package action handler does not support the 'all' target."
            << std::endl;
        return EXIT_FAILURE;
    }

    /* check PORTDIR */
    if (not util::is_dir(options::portdir()))
	throw FileException(options::portdir());

    /* fetch/parse herds.xml for info lookup */
    fetch_herdsxml();
    herdsxml.parse(options::herdsxml());

    /* fetch/parse devaway for marking away devs */
    if (options::devaway())
    {
        fetch_devawayxml();
        devaway.parse(options::devawayxml());
    }

    /* setup with regex */
    with.assign(options::dev() ? options::with_herd() :
                                 options::with_dev(),
                                 Regex::icase);

    /* load previously cached results */
    if (options::querycache())
        querycache.load();

    /* search previously cached results for current queries
     * and insert into our matches map if found */
    if (not querycache.empty())
    {
        for (opts_type::iterator i = opts.begin() ; i != opts.end() ; )
        {
            /* does a previously cached query that
             * matches our criteria exist? */
            querycache_T::iterator qi =
                querycache.find(pkgQuery_T(*i, with(), options::dev()));
            if (qi != querycache.end() and not querycache.is_expired(*qi))
            {
                debug_msg("found '%s' in query cache", i->c_str());

                matches.insert(std::make_pair(*i, new pkgQuery_T(*qi)));
                matches[*i]->query = *i;
                matches[*i]->with  = with();

                if (options::dev())
                {
                    matches[*i]->info.set_user(*i);
                    herdsxml.fill_developer(matches[*i]->info);
                }

                i = opts.erase(i);
            }
            /* is a wider-scoped query cached? */
            else if (qi == querycache.end() and not with.empty())
            {
                /* If so, use the results to narrow down what we need and
                 * partially load the metadata cache */
                qi = querycache.find(pkgQuery_T(*i, "", options::dev()));
                if (qi != querycache.end() and not querycache.is_expired(*qi))
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
                        pkgQuery_T q(*i, with(), options::dev());
                        q.date = std::time(NULL);
                        q.with = with();

                        if (options::dev())
                        {
                            q.info.set_user(*i);
                            herdsxml.fill_developer(q.info);
                        }

                        search(pkgs, q);
                        i = opts.erase(i);

                    } else ++i;
                } else ++i;
            } else ++i;
        }
    }

    if (options::debug())
    {
        debug_msg("opts.size() after querycache search = %d", opts.size());
        for (opts_type::iterator i = opts.begin() ; i != opts.end() ; ++i)
            std::cout << *i << std::endl;
    }

    at_least_one_not_cached = (not opts.empty());
    cache_is_valid = (options::metacache() and metacache.valid());

    if (cache_is_valid and at_least_one_not_cached)
    {
        try
        {
            metacache.load();
        }
        catch (const Exception)
        {
            return EXIT_FAILURE;
        }
    }
    else if (at_least_one_not_cached)
    {
        /* fill cache and dump to disk */
        metacache.fill();
        metacache.dump();
    }

    debug_msg("metacache.size() == %d", metacache.size());

    if (at_least_one_not_cached)
        search(opts);

    display();

    if (options::count())
    {
        output("", util::sprintf("%d", size));
        options::set_count(false); /* otherwise flush() will display size again */
    }
    else if (options::meta())
    {
        /* disable stuff we've handled already */
        options::set_regex(false);
        options::set_eregex(false);
        options::set_timer(false);

        /* get rid of any package dupes */
        std::sort(packages.begin(), packages.end());
        packages.erase(std::unique(packages.begin(), packages.end()),
            packages.end());

        /* ...and pass it to the metadata handler */
        action_meta_handler_T mhandler;
        mhandler(packages);
    }

    output.flush(*stream);

    if (not not_found.empty())
    {
        std::for_each(not_found.begin(), not_found.end(),
            std::bind2nd(Error(), &with));
    }

    if (options::timer())
    {
        *stream << std::endl << "Took " << elapsed << "ms to parse "
            << metacache.size() << " metadata.xml's ("
            << std::setprecision(4)
            << (static_cast<float>(elapsed) / metacache.size())
            << " ms/metadata.xml)." << std::endl
            << "Took " << herdsxml.elapsed() << "ms to parse herds.xml."
            << std::endl;
    }
    else if (options::verbose() and not options::quiet())
    {
        *stream << std::endl
            << "Parsed " << metacache.size() << " metadata.xml's." << std::endl;
    }

    if (options::querycache())
        querycache.dump();

    /* we handler timer here */
    options::set_timer(false);

    flush();
    cleanup();
    return EXIT_SUCCESS;
}

void
action_pkg_handler_T::cleanup()
{
    std::map<std::string, pkgQuery_T * >::iterator m;
    for (m = matches.begin() ; m != matches.end() ; ++m)
        delete m->second;
}

/* vim: set tw=80 sw=4 et : */
