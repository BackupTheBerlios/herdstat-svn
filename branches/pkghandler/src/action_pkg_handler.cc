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
    std::cerr << std::endl
        << "Failed to find any packages maintained by '"
        << criteria << "'";

    if (with.empty())
    {
        std::cerr << "." << std::endl;
        return;
    }
    
    std::cerr << " with " << (dev? "herd":"developer") << " '"
        << with << "'." << std::endl;
}

/*
 * binary predicates for searching metadata for
 * a regular expression via find_if().
 */

static bool
doesMatch(util::string s, util::regex_T *r) { return *r == s; }

static bool
doesHerdMatch(metadata_xml_T::herd_type::value_type m,
              util::regex_T *r)
{ return *r == util::get_user_from_email(m.first); }

/*
 * Determine whether or not the metadata.xml matches our
 * search criteria.
 */

bool
action_pkg_handler_T::metadata_matches(const metadata_xml_T &metadata,
                                       const util::string &criteria)
{
    if (dev)
    {
        if ((regex and (std::find_if(metadata.devs().begin(),
            metadata.devs().end(), std::bind2nd(
            std::ptr_fun(doesHerdMatch), &regexp)) != metadata.devs().end()) and
            (with.empty() or metadata.herd_exists(with))) or
            (not regex and metadata.dev_exists(criteria) and
            (with.empty() or metadata.herd_exists(with))))
            return true;
    }
    else
    {
        if ((regex and std::find_if(metadata.herds().begin(),
            metadata.herds().end(), std::bind2nd(
            std::ptr_fun(doesMatch), &regexp)) != metadata.herds().end()) or
            (not regex and metadata.herd_exists(criteria)))
        {
            if (with.empty())
                return true;
            else
            {
                if ((with == "none") and metadata.devs().empty())
                    return true;
                else if (metadata.dev_exists(with) or
                         metadata.dev_exists(with+"@gentoo.org"))
                    return true;
            }
        }
    }

    return false;
}

/*
 * Search the metadata list for those matching
 * the specified query object.  Mainly meant for
 * using a subset of the actual metadata list.
 */

void
action_pkg_handler_T::search(pkgQuery_T *q)
{
    if (regex)
        regexp.assign(q->query,
            eregex ? REG_EXTENDED|REG_ICASE : REG_ICASE);

    /* for each metadata.xml... */
    metadatas_T::const_iterator m;
    for (m = metadatas.begin() ; m != metadatas.end() ; ++m)
    {
        /* unfortunately, we still have to do a sanity check in the event
         * that the user has sync'd, a metadata.xml has been removed, and
         * the cache not yet updated ; otherwise we'll get a parser error */
        if (not util::is_file(*m))
            continue;

        /* parse metadata.xml */
        const metadata_xml_T metadata(*m);
        elapsed += metadata.elapsed();

        q->date = std::time(NULL);
        q->with = with;

        /* 
         * determine whether the package matches our search criteria
         */

        if (not metadata_matches(metadata, q->query))
            continue;

        if (dev)
        {
            q->type = QUERYTYPE_DEV;
            q->info = herds_xml.get_dev_info(q->query);
        }

        /* get category/package from absolute path */
        util::string package = m->substr(portdir.size() + 1);
        util::string::size_type pos = package.find("/metadata.xml");
        if (pos != util::string::npos)
            package = package.substr(0, pos);

        debug_msg("Match found in %s.", m->c_str());

        /* store it */
        (*q)[package] = metadata.longdesc();
    }

    /* cache query object */
    pkgcache(new pkgQuery_T(*q));
}

/*
 * Search every metadata.xml for the specified criteria.
 */

void
action_pkg_handler_T::search(const opts_type &opts)
{
    /* for each metadata.xml */
    metadatas_T::const_iterator m;
    for (m = metadatas.begin() ; m != metadatas.end() ; ++m)
    {
        if (status)
            ++progress;

        /* unfortunately, we still have to do a sanity check in the event
         * that the user has sync'd, a metadata.xml has been removed, and
         * the cache not yet updated ; otherwise we'll get a parser error */
        if (not util::is_file(*m))
            continue;

        /* parse metadata.xml */
        const metadata_xml_T metadata(*m);
        elapsed += metadata.elapsed();

        /* for each specified herd/dev */
        opts_type::iterator i;
        for (i = opts.begin() ; i != opts.end() ; ++i)
        {
            if (regex)
                regexp.assign(*i, eregex? REG_EXTENDED|REG_ICASE : REG_ICASE);

            if (not metadata_matches(metadata, *i))
                continue;

            debug_msg("Match found in %s.", m->c_str());

            /* get category/package from absolute path */
            util::string package = m->substr(portdir.size() + 1);
            util::string::size_type pos = package.find("/metadata.xml");
            if (pos != util::string::npos)
                package = package.substr(0, pos);

            /* we've already inserted at least one package */
            std::map<util::string, pkgQuery_T * >::iterator pos;
            if ((pos = matches.find(*i)) != matches.end())
                (*(pos->second))[package] = metadata.longdesc();
            /* nope, so create a new query object */
            else
            {
                pkgQuery_T *q = new pkgQuery_T(*i, with, dev);
                q->date = std::time(NULL);
            
                if (dev)
                    q->info = herds_xml.get_dev_info(*i);

                (*q)[package] = metadata.longdesc();
                matches[*i] = q;
            }
        }
    }

    /* cache results */
    std::map<util::string, pkgQuery_T * >::iterator i;
    for (i = matches.begin() ; i != matches.end() ; ++i)
        pkgcache(new pkgQuery_T(*(i->second)));
}

void
action_pkg_handler_T::display(pkgQuery_T *q)
{
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
        else
        {
            output("Herd", q->query);
            if (not herds_xml[q->query]->mail.empty())
                output("Email", herds_xml[q->query]->mail);
            if (not herds_xml[q->query]->desc.empty())
                output("Description",
                    util::tidy_whitespace(herds_xml[q->query]->desc));
        }

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

            if (pn != query.size())
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

void
action_pkg_handler_T::display()
{
    int n = 1;
    std::map<util::string, pkgQuery_T * >::iterator m;
    for (m = matches.begin() ; m != matches.end() ; ++m, ++n)
    {
        if (m->second->empty())
        {
            if (matches.size() == 1)
            {
                if (not quiet)
                    error(*i);

                pkgcache.dump();
                throw action_E();
            }

            not_found.push_back(m->first);
            continue;
        }

        if ((n == 1) and status and at_least_one_not_cached)
            output.endl();

        size += m->second->size();

        /* was --metadata also specified? if so, construct the package
         * list.  When we're all done, the list will be passed to
         * action_meta_handler_T::operator(). */
        if (meta and not count)
        {
            /* we're only interested in the package names */
            pkgQuery_T::iterator p;
            for (p = query.begin() ; p != query.end() ; ++p)
                packages.push_back(p->first);
        }
        else
        {
            display(m->second);

            /* only skip a line if we're not on the last one */
            if (not count and n != optsize)
                if (not quiet or (quiet and m->second->size() > 0))
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
    optsize = opts.size();

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
	throw bad_fileobject_E(portdir);

    /* load previously cached results */
    if (use_cache)
        pkgcache.load();

    /* fetch/parse herds.xml for info lookup */
    herds_xml.fetch();
    herds_xml.parse();

    /* set format attributes */
    output.set_maxlabel(16);
    output.set_maxdata(maxcol - output.maxlabel());
    output.set_attrs();

    /* load our map with any previously cached queries,
     * erasing them from opts as they're found. */
    if (not pkgcache.empty())
    {
        opts_type::iterator i;
        for (i = opts.begin() ; i != opts.end() ; ++i)
        {
            pkgCache_T::iterator p = pkgcache.find(pkgQuery_T(*i, with, dev));
            if (p != pkgcache.end() and not pkgcache.is_expired(*p))
            {
                /* exists and hasn't expired */
                debug_msg("found '%s' in cache", i->c_str());
                matches[*i] = new pkgQuery_T(*(*p));
                opts.erase(i);
            }
            else if (p == pkgcache.end() and not with.empty())
            {
                /* see if a wider-scoped query has been cached.  if so, use it
                 * to get a narrowed down list of metadata.xml's to parse. */
                pkgCache_T::iterator pc = pkgcache.find(pkgQuery_T(*i, "", dev));
                if (pc != pkgcache.end() and not pkgcache.is_expired(*pc))
                {
                    std::vector<util::string> mlist((*pc)->make_list(portdir));
                    metadatas.set(mlist);

                    pkgQuery_T q(*i, with, dev);
                    search(&q);
                    opts.erase(i);
                }
            }
        }
    }

    at_least_one_not_cached = (not opts.empty());

    /* get metadata.xml list */
    if (at_least_one_not_cached)
        metadatas.get(portdir);

    /* show pretty progress thinggy */
    if (status and at_least_one_not_cached)
    {
        *stream << "Parsing metadata.xml's: ";
        progress.start(metadatas.size());
    }

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
            << metadatas.size() << " metadata.xml's ("
            << std::setprecision(4)
            << (static_cast<float>(elapsed) / metadatas.size())
            << " ms/metadata.xml)." << std::endl
            << "Took " << herds_xml.elapsed() << "ms to parse herds.xml."
            << std::endl;
    }
    else if (verbose and not quiet)
    {
        *stream << std::endl
            << "Parsed " << metadatas.size() << " metadata.xml's." << std::endl;
    }

    if (at_least_one_not_cached)
        pkgcache.dump();

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
