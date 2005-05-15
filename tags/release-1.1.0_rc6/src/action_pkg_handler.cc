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
#include <fstream>
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

    if ((dev and with_herd.empty()) or (not dev and with_dev.empty()))
    {
        std::cerr << "." << std::endl;
        return;
    }
    
    std::cerr << " with ";

    if (dev)
        std::cerr << "herd '" << with_herd;
    else
        std::cerr << "developer '" << with_dev;
    
    std::cerr << "'." << std::endl;
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
action_pkg_handler_T::is_found(const metadata_xml_T &metadata,
                               const util::string &criteria)
{
    util::string with(dev ? with_herd : with_dev);

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
 * Loop through every metadata.xml in the tree,
 * searching for packages that match our search 
 * criteria.
 */

void
action_pkg_handler_T::search(package_list *list)
{
    if (regex)
        regexp.assign(list->name, eregex ? REG_EXTENDED|REG_ICASE : REG_ICASE);

    /* for every metadata.xml in the tree... */
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

        /* 
         * determine whether the package matches our search criteria
         */

        if (not is_found(metadata, list->name))
            continue;

        if (dev)
            list->info = herds_xml.get_dev_info(list->name);

        /* get category/package from absolute path */
        util::string package = m->substr(portdir.size() + 1);
        util::string::size_type pos = package.find("/metadata.xml");
        if (pos != util::string::npos)
            package = package.substr(0, pos);

        debug_msg("Match found in %s.", m->c_str());

        /* store it */
        (*list)[package] = metadata.longdesc();
    }
}

void
action_pkg_handler_T::display(package_list &list)
{
    if (not quiet)
    {
        if (regex)
            output("Regex", list.name);
        else if (dev)
        {
            if (list.info.name.empty())
                output("Developer", list.name);
            else
                output("Developer",
                    list.info.name + " (" + list.name + ")");

            output("Email", list.name + "@gentoo.org");
        }
        else
        {
            output("Herd", list.name);
            if (not herds_xml[list.name]->mail.empty())
                output("Email", herds_xml[list.name]->mail);
            if (not herds_xml[list.name]->desc.empty())
                output("Description",
                    util::tidy_whitespace(herds_xml[list.name]->desc));
        }

        if (list.empty())
            output("Packages(0)", "none");
        /* display first package on same line */
        else if (verbose and optget("color", bool))
            output(util::sprintf("Packages(%d)", list.size()),
                color[blue] + list.begin()->first + color[none]);
        else
            output(util::sprintf("Packages(%d)", list.size()),
                list.begin()->first);
    }
    else if (not list.empty() and not count)
        output("", list.begin()->first);

    /* display the category/package */
    package_list::iterator p = list.begin();
    if (not list.empty()) ++p;
    package_list::size_type pn = 1;
    for ( ; p != list.end() ; ++p, ++pn)
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

            if (pn != list.size())
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
 * Given a list of herds/devs, determine all packages belonging
 * to each herd/dev. For reliability reasons, every metadata.xml
 * in the tree is parsed.
 */

int
action_pkg_handler_T::operator() (opts_type &opts)
{
    opts_type not_found, packages;

    metadatas.set_portdir(portdir);

    herds_xml.fetch();
    herds_xml.parse();

    /* set format attributes */
    output.set_maxlabel(16);
    output.set_maxdata(maxcol - output.maxlabel());
    output.set_attrs();

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

    if (status)
    {
        *stream << "Parsing metadata.xml's: ";
        progress.start(opts.size() * metadatas.size());
    }

    /* for each specified herd/dev... */
    opts_type::size_type n = 1;
    opts_type::iterator i;
    for (i = opts.begin() ; i != opts.end() ; ++i, ++n)
    {
        package_list list(*i);

        search(&list);

        if ((n == 1) and status)
            output.endl();

        if (list.empty())
        {
            /* only display error if opts.size() == 1 */
            if (opts.size() == 1)
            {
                if (not quiet)
                    error(*i);

                return EXIT_FAILURE;
            }

            /* otherwise, save it and we'll deal with it later */
            not_found.push_back(*i);
            continue;
        }

        size += list.size();

        /* was --metadata also specified? if so, construct the package
         * list.  When we're all done, the list will be passed to
         * action_meta_handler_T::operator(). */
        if (meta and not count)
        {
            /* we're only interested in the package names */
            package_list::iterator p;
            for (p = list.begin() ; p != list.end() ; ++p)
                packages.push_back(p->first);
        }
        else
        {
            display(list);

            /* only skip a line if we're not on the last one */
            if (not count and n != opts.size())
                if (not quiet or (quiet and list.size() > 0))
                    output.endl();
        }
    }

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

    /* we handler timer here */
    timer = false;

    flush();
    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
