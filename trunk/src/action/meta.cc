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

#include <functional>

#include <herdstat/util/misc.hh>
#include <herdstat/util/string.hh>
#include <herdstat/portage/exceptions.hh>
#include <herdstat/portage/package_which.hh>
#include <herdstat/portage/util.hh>
#include <herdstat/portage/ebuild.hh>
#include <herdstat/portage/license.hh>
#include <herdstat/portage/metadata_xml.hh>

#include "common.hh"
#include "overlay_display.hh"
#include "action/meta.hh"

using namespace herdstat;
using namespace gui;

bool
MetaActionHandler::allow_pwd_query() const
{
    return true;
}

void
MetaActionHandler::handle_pwd_query(Query * const query,
                                    QueryResults * const results)
{
    const std::string pwd(util::getcwd());

    if (portage::is_pkg_dir(pwd))
    {
        matches.push_back(portage::Package(
                            portage::get_pkg_from_path(pwd),
                            util::dirname(util::dirname(pwd))));
    }
    else if (portage::is_category(pwd))
    {
        matches.push_back(portage::Package(util::basename(pwd),
                                           util::dirname(pwd)));
    }
    else
    {
        results->add("You must be in a package directory or category if you want to run the meta action handler with no arguments.");
        throw ActionException();
    }

    set_pwd_mode(true);
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
    BacktraceContext c("add_metadata("+data.pkg+")");

    const Options& options(GlobalOptions());
    const portage::MetadataXML m(data.path);
    const portage::Metadata& meta(m.data());
    const portage::Herds& herds(meta.herds());
    const portage::Developers& devs(meta.devs());

    if (not meta.is_category() and
        (herds.empty() or (herds.front() == "no-herd")))
        results->add("Herds(0)", "none");
    else if (not herds.empty())
        results->transform(util::sprintf("Herds(%d)", herds.size()),
                herds.begin(), herds.end(), util::tidy_whitespace);

    if (options.quiet())
    {
        if (devs.size() >= 1)
            results->transform(devs.begin(), devs.end(),
                std::mem_fun_ref(&portage::Developer::email));
        else if (not meta.is_category())
            results->add("none");
    }
    else
    {
        if (devs.size() >= 1)
            results->add(util::sprintf("Maintainers(%d)", devs.size()),
                    devs.front().email());

        if (devs.size() > 1)
            std::transform(++devs.begin(), devs.end(),
                std::back_inserter(*results),
                std::mem_fun_ref(&portage::Developer::email));
        else if (not meta.is_category() and devs.empty())
            results->add("Maintainers(0)", "none");
    }

    if (not meta.longdesc().empty())
        longdesc.assign(meta.longdesc());
}

static void
add_data(const metadata_data& data, QueryResults * const results,
         util::ProgressMeter *spinner)
{
    BacktraceContext c("add_data("+data.pkg+")");

    util::ColorMap& color(GlobalColorMap());
    Options& options(GlobalOptions());
    std::string longdesc;
    portage::Ebuild ebuild_vars;

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
        portage::PackageWhich which;
        const std::vector<std::string>& which_results(
                which(data.pkg, data.portdir, spinner));
        const std::string& ebuild(which_results.front());

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
                results->add("License", license.str());
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
                std::vector<std::string> parts;
                util::split(ebuild_vars["HOMEPAGE"], std::back_inserter(parts));

                if (parts.size() >= 1)
                    results->add("Homepage", parts.front());

                if (parts.size() > 1)
                    std::copy(parts.begin() + 1, parts.end(),
                        std::back_inserter(*results));
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
        {
            const std::string tidyld(util::tidy_whitespace(longdesc));
            debug_msg("long description = '%s'", tidyld.c_str());
            results->add("Description", tidyld);
        }
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
MetaActionHandler::do_results(Query& query, QueryResults * const results)
{
    BacktraceContext c("MetaActionHandler::do_results()");

    OverlayDisplay od(results);
    options.set_count(false);

    if (not options.regex() and not pwd_mode())
    {
        for (Query::iterator q = query.begin() ; q != query.end() ;
                ++q, increment_spinner())
        {
            try
            {
                const std::vector<portage::Package>& res(find().results());
                find()(q->second, spinner());
                if (is_ambiguous(res))
                    throw portage::AmbiguousPkg(res.begin(), res.end());
                /* not ambigious but more than one match.  that means we have 1
                 * in the real portdir, and at least 1 in an overlay.  See if
                 * we can find the one from the last-specified overlay and if
                 * not just show the first one from an overlay. */
                else if (res.size() > 1 and options.overlay() and
                        not GlobalOptions().overlays().empty())
                {
                    const std::string& overlay(GlobalOptions().overlays().back());

                    /* find first package whose portdir()
                     * member equals overlay */
                    std::vector<portage::Package>::const_iterator i =
                        std::find_if(res.begin(), res.end(),
                            util::compose_f_gx(
                                std::bind2nd(std::equal_to<std::string>(), overlay),
                                std::mem_fun_ref(&portage::Package::portdir)));

                    if (i == res.end())
                    {
                        /* otherwise just settle for the
                         * first one in an overlay */
                        i = std::find_if(res.begin(), res.end(),
                                std::mem_fun_ref(&portage::Package::in_overlay));
                        assert (i != res.end());
                    }

                    matches.push_back(*i);
                }
                else
                    matches.insert(matches.end(), res.begin(), res.end());

                find().clear_results();

                if (not options.overlay())
                {
                    remove_overlay_packages();

                    /* might be empty if the pkg only exists in an overlay */
                    if (matches.empty())
                        throw portage::NonExistentPkg(q->second);
                }
            }
            catch (const portage::AmbiguousPkg& e)
            {
                results->add(e.name() + " is ambiguous.  Possible matches are:");
                results->add_linebreak();

                std::for_each(e.packages().begin(), e.packages().end(),
                    std::bind2nd(ColorAmbiguousPkg(), results));
            
                if (query.size() == 1 and options.iomethod() == "stream")
                    throw ActionException();
            }
            catch (const portage::NonExistentPkg& e)
            {
                results->add(e.what());

                if (query.size() == 1 and options.iomethod() == "stream")
                    throw ActionException();
            }
        }
    }

    this->size() = matches.size();

    std::vector<portage::Package>::iterator m;
    for (m = matches.begin() ; m != matches.end() ; ++m, increment_spinner())
    {
        metadata_data data;
        data.portdir = m->portdir();
        data.pkg = m->full();

        data.path = data.portdir + "/" + data.pkg + "/metadata.xml";

        if (data.portdir != options.portdir() and not pwd_mode())
            od.insert(data.portdir);

        data.is_category = (data.pkg.rfind('/') == std::string::npos);

        if (m != matches.begin())
            results->add_linebreak();

        if (data.portdir == options.portdir() or pwd_mode())
            results->add(data.is_category ? "Category" : "Package", data.pkg);
        else
            results->add(data.is_category ? "Category" : "Package",
                    data.pkg + od[data.portdir]);

        add_data(data, results, spinner());
    }
}

/* vim: set tw=80 sw=4 fdm=marker et : */
