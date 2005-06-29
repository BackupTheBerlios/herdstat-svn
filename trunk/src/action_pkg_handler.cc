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

#include <iomanip>
#include <algorithm>
#include <functional>
#include <iterator>
#include <map>

#include "common.hh"
#include "action_meta_handler.hh"
#include "action_pkg_handler.hh"

/*
 * Show search failure message.
 */

void
action_pkg_handler_T::error(const util::string &criteria) const
{
    if (quiet)
        return;

    std::cerr
        << "Failed to find any packages maintained by '"
        << criteria << "'";

    if (with.empty())
    {
        std::cerr << "." << std::endl;
        return;
    }
    
    std::cerr << " with " << (dev? "herd":"developer") << " '"
        << with() << "'." << std::endl;
}

/*
 * binary predicate for searching metadata for
 * a regular expression via find_if().
 */

static bool
doesHerdMatch(metadata_T::herd_type::value_type m, util::regex_T *r)
{ return *r == util::get_user_from_email(m.first); }

/*
 * Determine whether or not the metadata.xml matches our
 * search criteria.
 */

bool
action_pkg_handler_T::metadata_matches(const metadata_T &metadata,
                                       const util::string &criteria)
{
    if (dev)
    {
        if ((regex and (std::find_if(metadata.devs.begin(),
            metadata.devs.end(), std::bind2nd(
            std::ptr_fun(doesHerdMatch), &regexp)) != metadata.devs.end()) and
            (with.empty() or metadata.herd_exists(with))) or
            (not regex and metadata.dev_exists(criteria) and
            (with.empty() or metadata.herd_exists(with))))
            return true;
    }
    else
    {
        if ((regex and std::find_if(metadata.herds.begin(),
            metadata.herds.end(), std::bind1st(util::regexMatch(), &regexp))
            != metadata.herds.end()) or (not regex and
            metadata.herd_exists(criteria)) or (criteria == "no-herd" and
            metadata.herds.empty()))
        {
            if (with.empty())
                return true;
            else
            {
                /* --no-maintainer was specified.  It's true if there are
                 * a) no maintainers, or b) the herd is listed as a maintainer.
                 */
                if (with() == "none")
                {
                    if (metadata.devs.empty() or
                        ((metadata.devs.size() == 1) and
                        metadata.dev_exists(criteria)
                        or metadata.dev_exists(criteria+"@gentoo.org")))
                        return true;
                }
                else if (metadata.dev_exists(with) or
                         metadata.dev_exists(with()+"@gentoo.org"))
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
    if (regex)
        regexp.assign(q.query,
            eregex? REG_EXTENDED|REG_ICASE : REG_ICASE);

    /* for each package in the vector */
    opts_type::const_iterator i;
    for (i = pkgs.begin() ; i != pkgs.end() ; ++i)
    {
        /* parse it's metadata.xml */
        const util::string path(portdir + "/" + *i + "/metadata.xml");
        const metadata_xml_T mxml(path);
        const metadata_T meta(mxml.data(portdir));

        /* does it match the criteria? */
        if (not metadata_matches(meta, q.query))
            continue;

        /* yep, so save it */
        q[meta.pkg] = meta.longdesc;
    }

    /* cache it */
    querycache(q);
    /* save it in results map */
    matches[q.query] = new pkgQuery_T(q);
}

/*
 * Search metadata cache for the specified list of items.
 */

void
action_pkg_handler_T::search(const opts_type &opts)
{
    /* for each metadata.xml */
    metacache_T::const_iterator m;
    for (m = metacache.begin() ; m != metacache.end() ; ++m)
    {
        /* for each specified herd/dev */
        opts_type::const_iterator i;
        for (i = opts.begin() ; i != opts.end() ; ++i)
        {
            if (regex)
                regexp.assign(*i, eregex? REG_EXTENDED|REG_ICASE : REG_ICASE);

            /* does it match the criteria? */
            if (metadata_matches(*m, *i))
            {
                debug_msg("Match found in %s.", m->path.c_str());

                /* we've already inserted at least one package */
                std::map<util::string, pkgQuery_T * >::iterator mpos;
                if ((mpos = matches.find(*i)) != matches.end())
                    (*(mpos->second))[m->pkg] = m->longdesc;
                /* nope, so create a new query object */
                else
                {
                    matches[*i] = new pkgQuery_T(*i, with(), dev);
                    matches[*i]->date = std::time(NULL);
                    (*(matches[*i]))[m->pkg] = m->longdesc;

                    if (dev)
                        matches[*i]->info = herds_xml.get_dev_info(*i);
                }
            }
            /* didn't match */
            else if (matches.find(*i) == matches.end())
            {
                matches[*i] = new pkgQuery_T(*i, with(), dev);
                matches[*i]->date = std::time(NULL);

                if (dev)
                    matches[*i]->info = herds_xml.get_dev_info(*i);
            }
        }
    }

    /* cache everything */
    std::map<util::string, pkgQuery_T * >::iterator p;
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

    if (not quiet)
    {
        if (regex)
            output("Regex", q->query);
        else if (dev)
        {
            if (q->info.name.empty())
                output("Developer", q->query);
            else
                output("Developer",
                    q->info.name + " (" + q->query + ")");

            output("Email", q->query + "@gentoo.org");
        }
        else if (herds_xml.exists(q->query))
        {
            output("Herd", q->query);
            if (not herds_xml[q->query]->mail.empty())
                output("Email", herds_xml[q->query]->mail);
            if (not herds_xml[q->query]->desc.empty())
                output("Description",
                    util::tidy_whitespace(herds_xml[q->query]->desc));
        }
        else
            output("Herd", q->query);


        if (q->empty())
            output("Packages(0)", "none");
        /* display first package on same line */
        else if (verbose and optget("color", bool))
            output(util::sprintf("Packages(%d)", q->size()),
                color[blue] + q->begin()->first + color[none]);
        else
            output(util::sprintf("Packages(%d)", q->size()),
                q->begin()->first);
    }
    else if (not q->empty() and not count)
        output("", q->begin()->first);

    /* display the category/package */
    pkgQuery_T::const_iterator p = q->begin();
    if (not q->empty()) ++p;
    pkgQuery_T::size_type pn = 1;
    for ( ; p != q->end() ; ++p, ++pn)
    {
        util::string longdesc;
            
        if (not p->second.empty())
            longdesc = util::tidy_whitespace(p->second);

        if ((verbose and not quiet) and not longdesc.empty())
        {
            if (output.size() > 1 and output.peek() != "")
                output.endl();

            if (optget("color", bool))
                output("", color[blue] + p->first + color[none]);
            else
                output("", p->first);

            output("", longdesc);
            debug_msg("longdesc(%s): '%s'", p->first.c_str(),
                longdesc.c_str());

            if (pn != q->size())
                output.endl();
        }
        else if (verbose and not quiet)
        {
            if (optget("color", bool))
                output("", color[blue] + p->first + color[none]);
            else
                output("", p->first);
        }
        else if (not count)
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
    if (not meta)
    {
        output.set_maxlabel(16);
        output.set_maxdata(maxcol - output.maxlabel());
        if (use_devaway)
            output.set_devaway(devaway.keys());
        output.set_attrs();
    }

    opts_type::size_type n = 1;
    std::map<util::string, pkgQuery_T * >::iterator m;
    for (m = matches.begin() ; m != matches.end() ; ++m, ++n)
    {
        if (m->second->empty())
        {
            --n;
            if (matches.size() == 1)
            {
                error(m->first);
                cleanup();
                throw action_E();
            }

            not_found.push_back(m->first);
            continue;
        }

        size += m->second->size();

        /* was --metadata also specified? if so, construct the package
         * list.  When we're all done, the list will be passed to
         * action_meta_handler_T::operator(). */
        if (meta and not count)
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
            if (not count and n != matches.size())
                if (not quiet) //or (quiet and m->second->size() > 0))
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
    if (all)
    {
        std::cerr << "Package action handler does not support the 'all' target."
            << std::endl;
        return EXIT_FAILURE;
    }
    /* only 1 regex allowed at a time */
    else if (regex and opts.size() > 1)
    {
        std::cerr << "You may only specify one regular expression."
            << std::endl;
        return EXIT_FAILURE;
    }

    /* check PORTDIR */
    if (not util::is_dir(portdir))
	throw util::bad_fileobject_E(portdir);

    /* fetch/parse herds.xml for info lookup */
    herds_xml.fetch();
    herds_xml.parse();

    /* fetch/parse devaway for marking away devs */
    if (use_devaway)
    {
        devaway.fetch();
        devaway.parse();
    }

    /* setup with regex */
    with.assign(dev? optget("with-herd", util::string) :
                     optget("with-maintainer", util::string), REG_ICASE);

    /* load previously cached results */
    if (optget("querycache", bool))
        querycache.load();

    /* search previously cached results for current queries
     * and insert into our matches map if found */
    if (not querycache.empty())
    {
        opts_type tmp(opts);
        opts_type::iterator i;
        for (i = tmp.begin() ; i != tmp.end() ; ++i)
        {
            /* does a previously cached query that
             * matches our criteria exist? */
            querycache_T::iterator qi =
                querycache.find(pkgQuery_T(*i, with(), dev));
            if (qi != querycache.end() and not querycache.is_expired(*qi))
            {
                debug_msg("found '%s' in query cache", i->c_str());

                matches[*i] = new pkgQuery_T(*qi);
                matches[*i]->query = *i;
                matches[*i]->with  = with();

                if (dev)
                    matches[*i]->info = herds_xml.get_dev_info(*i);

                opts.erase(std::find(opts.begin(), opts.end(), *i));
            }
            /* is a wider-scoped query cached? */
            else if (qi == querycache.end() and not with.empty())
            {
                /* If so, use the results to narrow down what we need and
                 * partially load the metadata cache */
                qi = querycache.find(pkgQuery_T(*i, "", dev));
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
                        pkgQuery_T q(*i, with(), dev);
                        q.date = std::time(NULL);
                        q.with = with();
                        if (dev) q.info = herds_xml.get_dev_info(*i);

                        search(pkgs, q);
                        opts.erase(std::find(opts.begin(), opts.end(), *i));
                    }
                }
            }
        }
    }

    if (debug)
    {
        debug_msg("opts.size() after querycache search = %d", optsize);
        opts_type::iterator i;
        for (i = opts.begin() ; i != opts.end() ; ++i)
            std::cout << *i << std::endl;
    }

    at_least_one_not_cached = (not opts.empty());

    if (cache_is_valid and at_least_one_not_cached)
    {
        try
        {
            metacache.load();
        }
        catch (const metacache_parse_E)
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

    if (count)
    {
        output("", util::sprintf("%d", size));
        count = false; /* otherwise flush() will display size again */
    }
    else if (meta)
    {
        /* disable stuff we've handled already */
        optset("regex", bool, false);
        optset("eregex", bool, false);
        optset("timer", bool, false);

        /* get rid of any package dupes */
        std::sort(packages.begin(), packages.end());
        packages.erase(std::unique(packages.begin(), packages.end()),
            packages.end());

        /* ...and pass it to the metadata handler */
        action_meta_handler_T mhandler;
        mhandler(packages);
    }

    output.flush(*stream);

    opts_type::iterator i;
    for (i = not_found.begin() ; i != not_found.end() ; ++i)
        error(*i);

    if (not not_found.empty())
        std::cerr << std::endl;

    if (timer)
    {
        *stream << std::endl << "Took " << elapsed << "ms to parse "
            << metacache.size() << " metadata.xml's ("
            << std::setprecision(4)
            << (static_cast<float>(elapsed) / metacache.size())
            << " ms/metadata.xml)." << std::endl
            << "Took " << herds_xml.elapsed() << "ms to parse herds.xml."
            << std::endl;
    }
    else if (verbose and not quiet)
    {
        *stream << std::endl
            << "Parsed " << metacache.size() << " metadata.xml's." << std::endl;
    }

    if (optget("querycache", bool))
        querycache.dump();

    /* we handler timer here */
    timer = false;

    flush();
    cleanup();
    return EXIT_SUCCESS;
}

void
action_pkg_handler_T::cleanup()
{
    std::map<util::string, pkgQuery_T * >::iterator m;
    for (m = matches.begin() ; m != matches.end() ; ++m)
        delete m->second;
}

/* vim: set tw=80 sw=4 et : */
