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
#include <iterator>
#include <map>
#include <vector>
#include <memory>
#include <string>
#include <cstdlib>

#include "common.hh"
#include "metadatas.hh"
#include "herds_xml.hh"
#include "metadata_xml_handler.hh"
#include "formatter.hh"
#include "action_pkg_handler.hh"

/*
 * Search herds.xml for the specified developer.
 * Return the developer's name if found or an empty
 * string otherwise.
 */

std::string
dev_name(herds_T &herds_xml, std::string &dev)
{
    for (herds_T::iterator h = herds_xml.begin() ; h != herds_xml.end() ; ++h)
    {
        herd_T::iterator d = herds_xml[h->first]->find(dev + "@gentoo.org");
        if (d != herds_xml[h->first]->end())
            return d->second->name;
    }

    return "";
}

/*
 * Given a list of herds/devs, determine all packages belonging
 * to each herd/dev. For reliability reasons, every metadata.xml
 * in the tree is parsed.
 */

int
action_pkg_handler_T::operator() (std::vector<std::string> &opts)
{
    util::color_map_T color;
    util::timer_T t;
    util::progress_T progress;
    std::map<std::string, std::string>::size_type size = 0;
    std::vector<std::string> not_found;
    std::vector<std::string>::iterator i;

    portage::config_T config(optget("portage.config", portage::config_T));
    const std::string portdir(config.portdir());
    std::ostream *stream = optget("outstream", std::ostream *);

    /* this code takes long enough as it is... no need for
     * a zillion calls to optget inside loops */
    const bool quiet = optget("quiet", bool);
    const bool verbose = optget("verbose", bool);
    const bool timer = optget("timer", bool);
    const bool count = optget("count", bool);
    const bool debug = optget("debug", bool);
    const bool dev = optget("dev", bool);
    const bool regex = optget("regex", bool);
    const bool eregex = optget("eregex", bool);
    const bool status = not quiet and not debug;

    /* set format attributes */
    formatter_T output;
    output.set_maxlabel(16);
    output.set_maxdata(optget("maxcol", std::size_t) - output.maxlabel());
    output.set_attrs();

    /* action_pkg_handler doesn't support the all target */
    if (optget("all", bool))
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

    herds_xml_T herds_xml;

    /* our list of metadata.xml's */
    metadatas_T metadatas(portdir);

    if (status)
    {
        *stream << "Parsing metadata.xml's: ";
        progress.start(opts.size() * metadatas.size());
    }

    /* for each specified herd/dev... */
    std::vector<std::string>::size_type n = 1;
    for (i = opts.begin() ; i != opts.end() ; ++i, ++n)
    {
        util::regex_T regexp;
        dev_attrs_T attr;
        std::map<std::string, std::string> pkgs;

        if (regex and eregex)
            regexp.assign(*i, REG_EXTENDED|REG_ICASE);
        else if (regex)
            regexp.assign(*i, REG_ICASE);

        if (timer)
            t.start();

        /* for each metadata.xml... */
        metadatas_T::iterator m;
        for (m = metadatas.begin() ; m != metadatas.end() ; ++m)
        {
            bool found = false;

            if (status)
                ++progress;

            /* unfortunately, we still have to do a sanity check in the event
             * that the user has sync'd, a metadata.xml has been removed, and
             * the cache not yet updated ; otherwise we'll get a parser error */
            if (not util::is_file(*m))
                continue;

            std::auto_ptr<MetadataXMLHandler_T>
                handler(new MetadataXMLHandler_T());
            XMLParser_T parser(&(*handler));

            /* parse it */
            parser.parse(*m);

            if (dev)
            {
                herd_T::iterator d;
                if (regex)
                {
                    /* search metadata.xml for all devs matching regex */
                    for (d = handler->devs.begin() ;
                         d != handler->devs.end() ; ++d)
                    {
                        if (regexp == util::get_user_from_email(d->first))
                        {
                            found = true;
                            break;
                        }
                    }

                    if (not found)
                        continue;
                }
                else
                {
                    /* search the metadata for our dev */
                    d = handler->devs.find(*i + "@gentoo.org");
                    if (d == handler->devs.end())
                        continue;
                    else
                    {
                        const std::string herd = optget("with-herd", std::string);
                    
                        if (not herd.empty())
                        {
                            if (std::find(handler->herds.begin(),
                                handler->herds.end(), herd) != handler->herds.end())
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
                        attr.begin());
                    attr.name = dev_name(herds_xml.herds(), *i);
                    attr.role = d->second->role;
                }
            }
            else
            {
                if (regex)
                {
                    /* search metadata.xml for all herds matching regex */
                    std::vector<std::string>::iterator h;
                    for (h = handler->herds.begin() ;
                         h != handler->herds.end()  ; ++h)
                    {
                        if (regexp == *h)
                        {
                            found = true;
                            break;
                        }
                    }

                    if (not found)
                        continue;
                }
                else
                {
                    /* search the metadata.xml for our herd */
                    if (std::find(handler->herds.begin(),
                        handler->herds.end(), *i) == handler->herds.end())
                        continue;
                    else
                        found = true;
                }
            }

            /* get category/package from absolute path */
            std::string cat_and_pkg = m->substr(portdir.size() + 1);
            std::string::size_type pos = cat_and_pkg.find("/metadata.xml");
            if (pos != std::string::npos)
                cat_and_pkg = cat_and_pkg.substr(0, pos);

            debug_msg("Match found in %s.", m->c_str());

            pkgs[cat_and_pkg] = handler->longdesc;
        }
        
        if (timer)
            t.stop();

        if (pkgs.empty())
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

        size += pkgs.size();

        if ((n == 1) and status)
            output.endl();

        if (not quiet)
        {
            if (regex)
                output("Regex", *i);
            else if (dev)
            {
                if (attr.name.empty())
                    output("Developer", *i);
                else
                    output("Developer",
                        attr.name + " (" + (*i) + ")");

                output("Email", *i + "@gentoo.org");
            }
            else
            {
                output("Herd", *i);
                if (not herds_xml[*i]->mail.empty())
                    output("Email", herds_xml[*i]->mail);
                if (not herds_xml[*i]->desc.empty())
                    output("Description", util::tidy_whitespace(herds_xml[*i]->desc));
            }

            if (pkgs.empty())
                output("Packages(0)", "none");
            /* display first package on same line */
            else if (verbose)
                output(util::sprintf("Packages(%d)", pkgs.size()),
                    color[blue] + pkgs.begin()->first + color[none]);
            else
                output(util::sprintf("Packages(%d)", pkgs.size()),
                    pkgs.begin()->first);
        }
        else if (not pkgs.empty() and not count)
            output("", pkgs.begin()->first);

        /* display the category/package */
        std::map<std::string, std::string>::iterator p =
            ( pkgs.empty() ? pkgs.begin() : ++(pkgs.begin()) );
        std::map<std::string, std::string>::size_type pn = 1;
        for ( ; p != pkgs.end() ; ++p, ++pn)
        {
            std::string longdesc;
            
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

                if (pn != pkgs.size())
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

        /* only skip a line if we're not on the last one */
        if (not count and n != opts.size())
            if (not quiet or (quiet and pkgs.size() > 0))
                output.endl();
    }

    if (count)
        output("", util::sprintf("%d", size));

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

    if (timer)
    {
        *stream << std::endl << "Took " << t.elapsed() << "ms to parse "
            << metadatas.size() << " metadata.xml's ("
            << std::setprecision(4)
            << (static_cast<float>(t.elapsed()) / metadatas.size())
            << " ms/metadata.xml)." << std::endl;
    }
    else if (verbose and not quiet)
    {
        *stream << std::endl
            << "Parsed " << metadatas.size() << " metadata.xml's." << std::endl;
    }

    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
