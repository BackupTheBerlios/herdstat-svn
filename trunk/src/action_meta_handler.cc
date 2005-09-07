/*
 * herdstat -- src/action_meta_handler.cc
 * $Id: action_meta_handler.cc 515 2005-09-04 11:41:38Z ka0ttic $
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

#include <map>
#include <utility>
#include <algorithm>
#include <iterator>

#include <herdstat/portage/exceptions.hh>
#include <herdstat/portage/find.hh>
#include <herdstat/portage/misc.hh>
#include <herdstat/portage/ebuild.hh>
#include <herdstat/portage/metdata_xml.hh>

#include "common.hh"
#include "overlaydisplay.hh"
#include "action_meta_handler.hh"

void
action_meta_handler_T::display(const metadata_data& data)
{
    /* does the metadata.xml exist? */
    if (util::is_file(data.path))
    {
        const metadata_xml metadata(data.path);
        display_metadata(metadata, *stream);
    }
    /* package or category exists, but metadata.xml doesn't */
    else
    {
        if (quiet or not optget("color", bool))
            output("", "No metadata.xml");
        else
            output("", color[red] + "No metadata.xml." + color[none]);
            
        /* at least show ebuild DESCRIPTION and HOMEPAGE */
        if (not data.is_category)
        {
            std::string ebuild;
            try
            {
                ebuild = portage::ebuild_which(data.portdir, data.pkg);
            }
            catch (const portage::NonExistentPkg)
            {
                ebuild = portage::ebuild_which(portdir, data.pkg);
            }
                
            portage::ebuild_T ebuild_vars(ebuild);

            if (quiet and ebuild_vars["HOMEPAGE"].empty())
                ebuild_vars["HOMEPAGE"] = "none";

            if (not ebuild_vars["HOMEPAGE"].empty())
            {
                /* it's possible to have more than one HOMEPAGE */
                if (ebuild_vars["HOMEPAGE"].find("://") != std::string::npos)
                {
                    std::vector<std::string> parts = util::split(ebuild_vars["HOMEPAGE"]);

                    if (parts.size() >= 1)
                        output("Homepage", parts.front());

                    if (parts.size() > 1)
                    {
                        std::vector<std::string>::iterator h;
                        for (h = ( parts.begin() + 1) ; h != parts.end() ; ++h)
                            output("", *h);
                    }
                }
                else
                    output("Homepage", ebuild_vars["HOMEPAGE"]);
            }

            if (quiet and ebuild_vars["DESCRIPTION"].empty())
                ebuild_vars["DESCRIPTION"] = "none";

            if (not ebuild_vars["DESCRIPTION"].empty())
                output("Description", ebuild_vars["DESCRIPTION"]);
        }
    }
}

/*
 * Given a vector of overlays, search them
 * for the specified package, returning a pair<overlay,package>.
 */

int
action_meta_handler_T::operator() (opts_type &opts)
{
    OverlayDisplay_T od;

    bool pwd = false;
    std::string dir;

    if (use_devaway)
    {
        devaway.fetch(optget("devaway.location", std::string));
        devaway.parse(optget("devaway.location", std::string));
    }

    output.set_maxlabel(16);
    output.set_maxdata(maxcol - output.maxlabel());
    output.set_quiet(quiet, " ");
    if (use_devaway)
        output.set_devaway(devaway.keys());
    output.set_attrs();

    /* we dont care about these */
    count = false;

    if (all)
    {
        std::cerr << "Metadata action handler does not support the 'all' target."
            << std::endl;
        return EXIT_FAILURE;
    }

    /* we can be called with 0 opts if we are currently
     * in a directory that contains a metadata.xml */
    if (opts.empty())
    {
        unsigned short depth = 0;

        /* are we in a package's directory? */
        if (portage::in_pkg_dir())
            depth = 2;
        /* not in pkgdir and metdata exists,
         * so assume we're in a category */
        else if (util::is_file("metadata.xml"))
            depth = 1;
        else
        {
            std::cerr << "You must be in a package directory or category if you" << std::endl
                << "want to run " << PACKAGE << " -m with no non-option arguments." << std::endl;
            return EXIT_FAILURE;
        }

        /* Loop, trimming each directory from the end until depth == 0 */
        std::string leftover;
        std::string path = util::getcwd();
        while (depth > 0)
        {
            std::string::size_type pos = path.rfind('/');
            if (pos != std::string::npos)
            {
                if (leftover.empty())
                    leftover = path.substr(pos + 1);
                else
                    leftover = path.substr(pos + 1) + "/" + leftover;

                path = path.substr(0, pos);
            }
            --depth;
        }

        /* now assign portdir to our path, treating the leftovers as the
         * category or category/package */
        pwd = true;
        dir = path;
        opts.push_back(leftover);
        
        debug_msg("set portdir to '%s'", dir.c_str());
        debug_msg("added '%s' to opts.", leftover.c_str());
    }
    else if (regex and opts.size() > 1)
    {
        std::cerr << "You may only specify one regular expression."
            << std::endl;
        return EXIT_FAILURE;
    }
    else if (regex)
    {
        util::regex_T::string_type re(opts.front());
        opts.clear();

        regexp.assign(re, eregex ? REG_EXTENDED|REG_ICASE : REG_ICASE);

        matches = portage::find_package_regex(config,
                    regexp, overlay, &search_timer);

        if (matches.empty())
        {
            std::cerr << "Failed to find any packages matching '" << re << "'."
                << std::endl;
            return EXIT_FAILURE;
        }
    }
        
    opts_type::iterator i;
    for (i = opts.begin() ; i != opts.end() ; ++i)
        matches.insert(std::make_pair(dir, *i));

    /* for each specified package/category... */
    std::multimap<std::string, std::string>::size_type n = 1;
    std::multimap<std::string, std::string>::iterator m;
    for (m = matches.begin() ; m != matches.end() ; ++m, ++n)
    {
        metadata_data data;
        data.portdir = dir;

        try
        {
            /* The only reason portdir should be set already is if
             * opts == 0 and portdir is set to $PWD */
            if (pwd)
                data.pkg = portage::find_package_in(data.portdir,
                                m->second, &search_timer);
            else if (regex and not m->first.empty())
            {
                data.dir = m->first;
                data.pkg = m->second;
            }
            else
            {
                std::pair<std::string, std::string> p =
                    portage::find_package(config, m->second,
                    overlay, &search_timer);
                data.dir = p.first;
                data.pkg = p.second;
            }
        }
        catch (const portage::AmbiguousPkg &e)
        {
            std::cerr << e.name()
                << " is ambiguous. Possible matches are: "
                << std::endl << std::endl;
            
            std::vector<std::string>::const_iterator i;
            for (i = e.packages.begin() ; i != e.packages.end() ; ++i)
            {
                if (quiet or not optget("color", bool))
                    std::cerr << *i << std::endl;
                else
                    std::cerr << color[green] << *i << color[none] << std::endl;
            }

            if (matches.size() == 1)
                return EXIT_FAILURE;
            else
                continue;
        }
        catch (const portage::NonExistentPkg &e)
        {
            std::cerr << m->second << " doesn't seem to exist." << std::endl;

            if (matches.size() == 1)
                return EXIT_FAILURE;
            else
                continue;
        }

        data.path = data.portdir + "/" + data.pkg + "/metadata.xml";

        /* are we in an overlay? */
        if (data.portdir != portdir and not pwd)
            od.insert(data.portdir);

        /* if no '/' exists, assume it's a category */
        data.is_category = (data.pkg.find('/') == std::string::npos);

        if (n != 1)
            output.endl();

        if (data.portdir == portdir or pwd)
            output(data.is_category ? "Category" : "Package", data.pkg);

        /* it's in an overlay, so show a little thinggy to mark it as such */
        else
            output(data.is_category ? "Category" : "Package",
                    data.pkg + od[data.portdir]);

        display(data);
    }

    flush();
    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
