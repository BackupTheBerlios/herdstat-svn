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
#include "metadatas.hh"
#include "herds_xml.hh"
#include "metadata_xml.hh"
#include "formatter.hh"
#include "action_pkg_handler.hh"

/*
 * Search herds.xml for the specified developer.
 * Return the developer's name if found or an empty
 * string otherwise.
 */

static util::string
dev_name(const herds_xml_T &herds_xml, const util::string &dev)
{
    herds_xml_T::const_iterator h;
    for (h = herds_xml.begin() ; h != herds_xml.end() ; ++h)
    {
        herds_xml_T::herd_type::iterator d =
            herds_xml[h->first]->find(dev + "@gentoo.org");
        if (d != herds_xml[h->first]->end())
            return d->second->name;
    }

    return "";
}

/*
 * binary predicate for searching metadata for
 * a regular expression via find_if().
 */

static bool
doesMatch(util::string s, util::regex_T *r) { return *r == s; }

/*
 * Loop through every metadata.xml in the tree,
 * searching for packages that match our search 
 * criteria.
 */

void
action_pkg_handler_T::search(package_list *list)
{
    if (regex and eregex)
        regexp.assign(list->name, REG_EXTENDED|REG_ICASE);
    else if (regex)
        regexp.assign(list->name, REG_ICASE);

    /* for every metadata.xml in the tree... */
    metadatas_T::const_iterator m;
    for (m = metadatas.begin() ; m != metadatas.end() ; ++m)
    {
        bool found = false;
        metadata_xml_T::herd_type devs;
        metadata_xml_T::herds_type herds;
        metadata_xml_T::string_type longdesc;

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

        if (dev)
        {
            const util::string herd = optget("with-herd", util::string);
            metadata_xml_T::herd_type::iterator d;

            if (regex)
            {
                /* search metadata.xml for all devs matching regex */
                for (d = metadata.devs().begin() ;
                     d != metadata.devs().end() ; ++d)
                {
                    if (regexp == util::get_user_from_email(d->first))
                    {
                        /* matches regex and --with-herd? */
                        if (not herd.empty())
                        {
                            if (std::find(metadata.herds().begin(),
                                    metadata.herds().end(),
                                    herd) != metadata.herds().end())
                                found = true;
                        }
                        else
                            found = true;

                        if (found)
                            break;
                    }
                }
            }
            else
            {
                /* search the metadata for our dev */
                d = metadata.devs().find(list->name + "@gentoo.org");
                if (d != metadata.devs().end())
                {
                    if (not herd.empty())
                    {
                        if (std::find(metadata.herds().begin(),
                            metadata.herds().end(), herd) != metadata.herds().end())
                        {
                            found = true;
                        }
                    }
                    else
                        found = true;
                }
            }

            if (found)
            {
                std::copy(d->second->begin(), d->second->end(),
                    list->attr.begin());
                list->attr.name = dev_name(herds_xml, list->name);
                list->attr.role = d->second->role;
            }
        }
        else
        {
            found = (
                regex ?
                    /* yes */
                    std::find_if(metadata.herds().begin(),
                        metadata.herds().end(), std::bind2nd(
                        std::ptr_fun(doesMatch), &regexp)) != metadata.herds().end() :

                    /* no */
                    std::find(metadata.herds().begin(), metadata.herds().end(),
                        list->name) != metadata.herds().end()
            );
        }

        if (not found)
            continue;

        /* get category/package from absolute path */
        util::string package = m->substr(portdir.size() + 1);
        util::string::size_type pos = package.find("/metadata.xml");
        if (pos != util::string::npos)
            package = package.substr(0, pos);

        debug_msg("Match found in %s.", m->c_str());

        (*list)[package] = longdesc;
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
            if (list.attr.name.empty())
                output("Developer", list.name);
            else
                output("Developer",
                    list.attr.name + " (" + list.name + ")");

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
        else if (verbose)
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
    std::vector<util::string> not_found;

    metadatas.set_portdir(portdir);

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

        if (list.empty())
        {
            /* only display error if opts.size() == 1 */
            if (opts.size() == 1)
            {
                if (not quiet)
                {
                    if (dev)
                    {
                        std::cerr << std::endl
                            << "Failed to find developer '" << *i
                            << "' in any metadata.xml's." << std::endl;
                    }
                    else
                    {
                        std::cerr << std::endl
                            << "Failed to find herd '" << *i
                            << "' in any metadata.xml's." << std::endl;
                    }
                }

                return EXIT_FAILURE;
            }

            /* otherwise, save it and we'll deal with it later */
            not_found.push_back(*i);
            continue;
        }

        size += list.size();

        if ((n == 1) and status)
            output.endl();

        display(list);

        /* only skip a line if we're not on the last one */
        if (not count and n != opts.size())
            if (not quiet or (quiet and list.size() > 0))
                output.endl();
    }

    if (count)
    {
        output("", util::sprintf("%d", size));
        count = false;
    }

    output.flush(*stream);

    for (i = not_found.begin() ; i != not_found.end() ; ++i)
    {
        if (dev)
            std::cerr << std::endl
                << "Failed to find developer '" << *i <<
                "' in any metadata.xml's.";
        else
            std::cerr << std::endl
                << "Failed to find herd '" << *i <<
                "' in any metadata.xml's.";
    }

    if (not not_found.empty())
        std::cerr << std::endl;

    if (optget("timer", bool))
    {
        *stream << std::endl << "Took " << elapsed << "ms to parse "
            << metadatas.size() << " metadata.xml's ("
            << std::setprecision(4)
            << (static_cast<float>(elapsed) / metadatas.size())
            << " ms/metadata.xml)." << std::endl
            << "Took " << herds_xml.elapsed() << "ms to parse herds.xml."
            << std::endl;
    }
    else if (optget("verbose", bool) and not quiet)
    {
        *stream << std::endl
            << "Parsed " << metadatas.size() << " metadata.xml's." << std::endl;
    }

    flush();
    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
