/*
 * herdstat -- src/action_meta_handler.cc
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

#include <ostream>
#include <map>
#include <utility>
#include <algorithm>
#include <iterator>

#include "common.hh"
#include "formatter.hh"
#include "overlaydisplay.hh"
#include "metadata_xml.hh"
#include "action_meta_handler.hh"

struct mdata
{
    bool cat;
    util::string metadata;
    util::string portdir;
    util::string real_portdir;
    util::string package;
};

static void
display_metadata(const mdata &data)
{
    formatter_T output;
    util::color_map_T color;
    metadata_xml_T::herd_type  devs;
    metadata_xml_T::herds_type herds;
    util::string longdesc;

    const bool quiet = optget("quiet", bool);

    /* does the metadata.xml exist? */
    if (util::is_file(data.metadata))
    {
        util::vars_T ebuild_vars;

        /* parse it */
        {
            const metadata_xml_T meta(data.metadata);
            herds    = meta.herds();
            devs     = meta.devs();
            longdesc = meta.longdesc();
        }

        /* herds */
        if (not data.cat and (herds.empty() or (herds.front() == "no-herd")))
            output("Herds(0)", "none");
        else if (not herds.empty())
            output(util::sprintf("Herds(%d)", herds.size()), herds);

        /* devs */
        if (quiet)
        {
            if (devs.size() >= 1)
                output("", devs.keys());
            else if (not data.cat)
                output("", "none");
        }
        else
        {
            if (devs.size() >= 1)
                output(util::sprintf("Maintainers(%d)", devs.size()),
                    devs.keys().front());
            
            if (devs.size() > 1)
            {
                std::vector<metadata_xml_T::herd_type::key_type>
                    dev_keys(devs.keys());
                std::vector<metadata_xml_T::herd_type::key_type>::iterator d;
                for (d = ( dev_keys.begin() + 1 ); d != dev_keys.end() ; ++d)
                    output("", *d);
            }
            else if (not data.cat and devs.empty())
                output("Maintainers(0)", "none");
        }

        if (not data.cat)
        {
            util::string ebuild;
            try
            {
                ebuild = portage::ebuild_which(data.portdir, data.package);
            }
            catch (const portage::nonexistent_pkg_E)
            {
                ebuild = portage::ebuild_which(data.real_portdir, data.package);
            }
                
            ebuild_vars.read(ebuild);

            if (quiet and ebuild_vars["HOMEPAGE"].empty())
                ebuild_vars["HOMEPAGE"] = "none";

            if (not ebuild_vars["HOMEPAGE"].empty())
            {
                /* it's possible to have more than one HOMEPAGE */
                std::vector<util::string> parts =
                    util::split(ebuild_vars["HOMEPAGE"]);

                if (parts.size() >= 1)
                    output("Homepage", parts.front());

                if (parts.size() > 1)
                {
                    std::vector<util::string>::iterator h;
                    for (h = ( parts.begin() + 1) ; h != parts.end() ; ++h)
                        output("", *h);
                }
            }
        }

        /* long description */
        if (longdesc.empty())
        {
            if (not data.cat)
            {
                if (ebuild_vars["DESCRIPTION"].empty())
                    ebuild_vars["DESCRIPTION"] = "none";
                    
                output("Description", ebuild_vars["DESCRIPTION"]);
            }
            else
                output("Description", "none");
        }
        else
            output("Description", util::tidy_whitespace(longdesc));
    }

    /* package or category exists, but metadata.xml doesn't */
    else
    {
        if (quiet)
            output("", "No metadata.xml");
        else
            output("", color[red] + "No metadata.xml." + color[none]);
            
        /* at least show ebuild DESCRIPTION and HOMEPAGE */
        if (not data.cat)
        {
            util::string ebuild;
            try
            {
                ebuild = portage::ebuild_which(data.portdir, data.package);
            }
            catch (const portage::nonexistent_pkg_E)
            {
                ebuild = portage::ebuild_which(data.real_portdir, data.package);
            }
                
            util::vars_T ebuild_vars(ebuild);

            if (quiet and ebuild_vars["HOMEPAGE"].empty())
                ebuild_vars["HOMEPAGE"] = "none";

            if (not ebuild_vars["HOMEPAGE"].empty())
            {
                std::vector<util::string> parts =
                    util::split(ebuild_vars["HOMEPAGE"]);

                if (parts.size() >= 1)
                    output("Homepage", parts.front());

                if (parts.size() > 1)
                {
                    std::vector<util::string>::iterator h;
                    for (h = ( parts.begin() + 1) ; h != parts.end() ; ++h)
                        output("", *h);
                }
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
    std::ostream *stream = optget("outstream", std::ostream *);
    const bool quiet = optget("quiet", bool);
    const bool regex = optget("regex", bool);
    const bool overlay = optget("overlay", bool);
    portage::config_T config(optget("portage.config", portage::config_T));
    std::multimap<util::string, util::string> matches;

    util::color_map_T color;
    bool pwd = false;
    const util::string real_portdir(config.portdir());
    util::string portdir;
    OverlayDisplay_T od;

    formatter_T output;
    output.set_maxlabel(16);
    output.set_maxdata(optget("maxcol", std::size_t) - output.maxlabel());
    output.set_quiet(quiet, " ");
    output.set_attrs();

    /* we dont care about these */
    optset("verbose", bool, false);
    optset("timer", bool, false);

    if (optget("all", bool))
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
        util::string leftover;
        util::string path = util::getcwd();
        while (depth > 0)
        {
            util::string::size_type pos = path.rfind('/');
            if (pos != util::string::npos)
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
        portdir = path;
        opts.push_back(leftover);
        
        debug_msg("set portdir to '%s'", portdir.c_str());
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
        util::regex_T regexp;
        util::regex_T::string_type re(opts.front());
        opts.clear();

        if (optget("eregex", bool))
            regexp.assign(re, REG_EXTENDED|REG_ICASE);
        else
            regexp.assign(re, REG_ICASE);

        matches = portage::find_package_regex(config, regexp, overlay);
        if (matches.empty())
        {
            std::cerr << "Failed to find any packages matching '" << re << "'."
                << std::endl;
            return EXIT_FAILURE;
        }
    }

    opts_type::iterator i;
    for (i = opts.begin() ; i != opts.end() ; ++i)
        matches.insert(std::make_pair(portdir, *i));

    /* for each specified package/category... */
    std::multimap<util::string, util::string>::iterator m;
    std::multimap<util::string, util::string>::size_type n = 1;
    for (m = matches.begin() ; m != matches.end() ; ++m, ++n)
    {
        mdata data;
        data.portdir = portdir;
        data.real_portdir = real_portdir;

        try
        {
            /* The only reason portdir should be set already is if
             * opts == 0 and portdir is set to $PWD */
            if (pwd)
                data.package = portage::find_package_in(data.portdir, m->second);
            else if (regex and not m->first.empty())
            {
                data.portdir = m->first;
                data.package = m->second;
            }
            else
            {
                std::pair<util::string, util::string> p =
                    portage::find_package(config, m->second, overlay);
                data.portdir = p.first;
                data.package = p.second;
            }
        }
        catch (const portage::ambiguous_pkg_E &e)
        {
            std::cerr << e.name()
                << " is ambiguous. Possible matches are: "
                << std::endl << std::endl;
            
            std::vector<util::string>::const_iterator i;
            for (i = e.packages.begin() ; i != e.packages.end() ; ++i)
            {
                if (quiet or not optget("color", bool))
                    std::cerr << m->second << std::endl;
                else
                    std::cerr << color[green] << m->second << color[none] << std::endl;
            }

            if (matches.size() == 1)
                return EXIT_FAILURE;
            else
                continue;
        }
        catch (const portage::nonexistent_pkg_E &e)
        {
            std::cerr << m->second << " doesn't seem to exist." << std::endl;

            if (matches.size() == 1)
                return EXIT_FAILURE;
            else
                continue;
        }

        data.metadata = data.portdir + "/" + data.package + "/metadata.xml";

        /* are we in an overlay? */
        if (data.portdir != data.real_portdir and not pwd)
            od.insert(data.portdir);

        /* if no '/' exists, assume it's a category */
        data.cat = (data.package.find('/') == util::string::npos);

        if (n != 1)
            output.endl();

        if (data.portdir == data.real_portdir or pwd)
            output(data.cat ? "Category" : "Package", data.package);

        /* it's in an overlay, so show a little thinggy to mark it as such */
        else
            output(data.cat ? "Category" : "Package",
                data.package + od[data.portdir]);

        display_metadata(data);
    }

    output.flush(*stream);
    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
