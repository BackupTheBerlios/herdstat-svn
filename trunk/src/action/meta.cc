/*
 * herdstat -- src/action/meta.cc
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

#include <herdstat/util/string.hh>
#include <herdstat/portage/exceptions.hh>
#include <herdstat/portage/find.hh>
#include <herdstat/portage/misc.hh>
#include <herdstat/portage/ebuild.hh>
#include <herdstat/portage/license.hh>
#include <herdstat/portage/metadata_xml.hh>

#include "common.hh"
#include "overlaydisplay.hh"
#include "action/meta.hh"

using namespace herdstat;
using namespace gui;

bool
MetaActionHandler::allow_empty_query() const
{
    return true;
}

const char * const
MetaActionHandler::id() const
{
    return "meta";
}

const char * const
MetaActionHandler::desc() const
{
    return "Get metadata information for the given category/package.";
}

const char * const
MetaActionHandler::usage() const
{
    return "meta <package(s)|categor(y|ies)>";
}

Tab *
MetaActionHandler::createTab(WidgetFactory *widgetFactory)
{
    Tab *tab = widgetFactory->createTab();
    tab->set_title(this->id());

    return tab;
}

struct metadata_data
{
    std::string path;
    std::string portdir;
    std::string pkg;
    bool is_category;
};

static void
add_metadata(const metadata_data& data, std::string& longdesc,
             QueryResults * const results)
{
    const Options& options(GlobalOptions());
    const portage::metadata_xml m(data.path);
    const portage::metadata& meta(m.data());
    const portage::Herds& herds(meta.herds());
    const portage::Developers& devs(meta.devs());

    if (not meta.is_category() and
        (herds.empty() or (herds.front() == "no-herd")))
        results->add("Herds(0)", "none");
    else if (not herds.empty())
        results->add(util::sprintf("Herds(%d)", herds.size()), herds);

    if (options.quiet())
    {
        std::vector<std::string> qdevs;
        portage::Developers::const_iterator d;
        for (d = devs.begin() ; d != devs.end() ; ++d)
            qdevs.push_back(d->email());

        if (devs.size() >= 1)
            results->add(qdevs);
        else if (not meta.is_category())
            results->add("none");
    }
    else
    {
        if (devs.size() >= 1)
            results->add(util::sprintf("Maintainers(%d)", devs.size()),
                    devs.front().email());

        if (devs.size() > 1)
        {
            portage::Developers::const_iterator d;
            for (d = ++devs.begin() ; d != devs.end() ; ++d)
                results->add(d->email());
        }
        else if (not meta.is_category() and devs.empty())
            results->add("Maintainers(0)", "none");
    }

    if (not meta.longdesc().empty())
        longdesc.assign(meta.longdesc());
}

static void
add_data(const metadata_data& data, QueryResults * const results)
{
    util::ColorMap& color(GlobalColorMap());
    Options& options(GlobalOptions());
    std::string longdesc;
    portage::ebuild ebuild_vars;

    if (util::is_file(data.path))
        add_metadata(data, longdesc, results);
    else
    {
        if (options.quiet() or not options.color())
            results->add("No metadata.xml.");
        else
            results->add(color[red] + "No metadata.xml." + color[none]);
    }

    if (not data.is_category)
    {
        std::string ebuild;
        try
        {
            ebuild = portage::ebuild_which(data.portdir, data.pkg);
        }
        catch (const portage::NonExistentPkg)
        {
            ebuild = portage::ebuild_which(options.portdir(), data.pkg);
        }

        assert(not ebuild.empty());
        ebuild_vars.read(ebuild);

        if (options.quiet() and ebuild_vars["LICENSE"].empty())
            ebuild_vars["LICENSE"] = "none";

        if (not ebuild_vars["LICENSE"].empty())
        {
            try
            {
                portage::License
                    license(ebuild_vars["LICENSE"], options.qa());
                results->add("License", license);
            }
            catch (const portage::QAException& e)
            {
                std::string err("Invalid license '");
                err += e.what();
                err += "' for " + data.pkg;
                throw portage::QAException(err);
            }
        }

        if (options.quiet() and ebuild_vars["HOMEPAGE"].empty())
            ebuild_vars["HOMEPAGE"] = "none";

        if (not ebuild_vars["HOMEPAGE"].empty())
        {
            /* it's possible to have more than one HOMEPAGE */
            if (ebuild_vars["HOMEPAGE"].find("://") != std::string::npos)
            {
                std::vector<std::string> parts = 
                    util::split(ebuild_vars["HOMEPAGE"]);

                if (parts.size() >= 1)
                    results->add("Homepage", parts.front());

                if (parts.size() > 1)
                {
                    std::vector<std::string>::iterator h;
                    for (h = ( parts.begin() + 1) ; h != parts.end() ; ++h)
                        results->add(*h);
                }
            }
            else
                results->add("Homepage", ebuild_vars["HOMEPAGE"]);
        }
        
        if (longdesc.empty())
        {
            if (options.quiet() and ebuild_vars["DESCRIPTION"].empty())
                ebuild_vars["DESCRIPTION"] = "none";

            if (not ebuild_vars["DESCRIPTION"].empty())
                results->add("Description", ebuild_vars["DESCRIPTION"]);
        }
        else
            results->add("Description", util::tidy_whitespace(longdesc));
    }
    else
    {
        if (longdesc.empty())
            results->add("Description", "none");
        else
            results->add("Description", util::tidy_whitespace(longdesc));
    }
}

void
MetaActionHandler::operator()(const Query& qq,
                              QueryResults * const results)
{
    Query query(qq);
    OverlayDisplay od(results);
    bool pwd = false;
    std::string dir;

    options.set_count(false);

    if (query.empty())
    {
        unsigned short depth = 0;

        /* are we in a package directory? */
        if (portage::in_pkg_dir())
            depth = 2;
        /* nope but a metadata exists, so assume we're in a category */
        else if (util::is_file("metadata.xml"))
            depth = 1;
        else
        {
            results->add("You must be in a package directory or category if you want to run the meta action handler with no arguments.");
            throw ActionException();
        }

        std::string leftover;
        std::string path(util::getcwd());
        while (depth > 0)
        {
            std::string::size_type pos = path.rfind('/');
            if (pos != std::string::npos)
            {
                leftover = (leftover.empty() ?
                        path.substr(pos + 1) :
                        path.substr(pos + 1) + "/" + leftover);
                path.erase(pos);
            }
            --depth;
        }

        pwd = true;
        dir = path;
        query.push_back(std::make_pair("", leftover));
    }
    else if (options.regex())
    {
        regexp.assign(query.front().second);
        query.clear();
        std::vector<std::string> rvec;

        matches = portage::find_package_regex(regexp, options.overlay(),
                        &search_timer);

        if (matches.empty())
        {
            results->add(util::sprintf("Failed to find any packages matching '%s'.",
                        regexp().c_str()));
            throw ActionException();
        }
    }

    Query::iterator i;
    for (i = query.begin() ; i != query.end() ; ++i)
        matches.insert(std::make_pair(dir, i->second));

    std::multimap<std::string, std::string>::size_type n = 1;
    std::multimap<std::string, std::string>::iterator m;
    for (m = matches.begin() ; m != matches.end() ; ++m, ++n)
    {
        metadata_data data;
        data.portdir = dir;

        try
        {
            if (pwd)
                data.pkg = portage::find_package_in(data.portdir,
                                m->second, &search_timer);
            else if (options.regex() and not m->first.empty())
            {
                data.portdir = m->first;
                data.pkg = m->second;
            }
            else
            {
                std::pair<std::string, std::string> p =
                    portage::find_package(m->second, options.overlay(),
                        &search_timer);
                data.portdir = p.first;
                data.pkg = p.second;
            }
        }
        catch (const portage::AmbiguousPkg &e)
        {
            results->add(e.name() + " is ambiguous.  Possible matches are: ");
            results->add_linebreak();
            results->add_linebreak();

            std::vector<std::string>::const_iterator i;
            for (i = e.packages.begin() ; i != e.packages.end() ; ++i)
            {
                if (options.quiet() or not options.color())
                    results->add(*i);
                else
                    results->add(color[green] + (*i) + color[none]);
            }

            if (matches.size() == 1 and options.iomethod() == "stream")
                throw ActionException();
            else
                continue;
        }
        catch (const portage::NonExistentPkg& e)
        {
            results->add(m->second + " doesn't seem to exist.");

            if (matches.size() == 1 and options.iomethod() == "stream")
                throw ActionException();
            else
                continue;
        }

        data.path = data.portdir + "/" + data.pkg + "/metadata.xml";

        if (data.portdir != options.portdir() and not pwd)
            od.insert(data.portdir);

        data.is_category = (data.pkg.rfind('/') == std::string::npos);

        if (n != 1)
            results->add_linebreak();

        if (data.portdir == options.portdir() or pwd)
            results->add(data.is_category ? "Category" : "Package", data.pkg);
        else
            results->add(data.is_category ? "Category" : "Package",
                    data.pkg + od[data.portdir]);

        add_data(data, results);
    }

    PortageSearchActionHandler::operator()(query, results);
}

/* vim: set tw=80 sw=4 fdm=marker et : */
