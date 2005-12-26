/*
 * herdstat -- src/action/dev.cc
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

#include <herdstat/util/algorithm.hh>
#include <herdstat/util/functional.hh>

#include "common.hh"
#include "action/dev.hh"

using namespace herdstat;
using namespace gui;

const char * const
DevActionHandler::id() const
{
    return "dev";
}

const char * const
DevActionHandler::desc() const
{
    return "Get information about the given developer(s).";
}

const char * const
DevActionHandler::usage() const
{
    return "dev <developer(s)>";
}

Tab *
DevActionHandler::createTab(WidgetFactory *widgetFactory)
{
    Tab *tab = widgetFactory->createTab();
    tab->set_title(this->id());

    return tab;
}

void
DevActionHandler::generate_completions(std::vector<std::string> *v) const
{
    portage::UserinfoXML& userinfo_xml(GlobalUserinfoXML());

    if (userinfo_xml.empty())
    {
        const portage::Herds& herds(GlobalHerdsXML().herds());
        portage::Herds::const_iterator h;

        for (h = herds.begin() ; h != herds.end() ; ++h)
            std::transform(h->begin(), h->end(),
                std::back_inserter(*v),
                std::mem_fun_ref(&portage::Developer::user));
    }
    else
    {
        const portage::Developers& devs(userinfo_xml.devs());
        std::transform(devs.begin(), devs.end(),
            std::back_inserter(*v),
            std::mem_fun_ref(&portage::Developer::user));
    }
}

void
DevActionHandler::do_init(Query& query, QueryResults * const results)
{
    BacktraceContext c("DevActionHandler::do_init()");

    ActionHandler::do_init(query, results);

    if (not options.userinfoxml().empty())
        GlobalUserinfoXML().parse(options.userinfoxml());
}

void
DevActionHandler::do_all(Query& query,
                         QueryResults * const results LIBHERDSTAT_UNUSED)
{
    BacktraceContext c("DevActionHandler::do_all()");

    portage::UserinfoXML& userinfo_xml(GlobalUserinfoXML());
    const portage::Developers& devs(userinfo_xml.devs());
    const portage::Herds& herds(GlobalHerdsXML().herds());
    portage::Herds::const_iterator h;

    for (h = herds.begin() ; h != herds.end() ; ++h)
        std::transform(h->begin(), h->end(),
            std::back_inserter(query),
            std::mem_fun_ref(&portage::Developer::user));

    if (not userinfo_xml.empty())
        std::transform(devs.begin(), devs.end(),
            std::back_inserter(query),
            std::mem_fun_ref(&portage::Developer::user));

    /* nuke dupes */
    std::sort(query.begin(), query.end(),
        util::compose_f_gx_hy(
            std::less<std::string>(),
            util::Second<Query::value_type>(),
            util::Second<Query::value_type>()));

    query.erase(std::unique(query.begin(), query.end(),
                    util::compose_f_gx_hy(
                        std::equal_to<std::string>(),
                        util::Second<Query::value_type>(),
                        util::Second<Query::value_type>())),
            query.end());
}

void
DevActionHandler::do_regex(Query& query, QueryResults * const results)
{
    BacktraceContext c("DevActionHandler::do_regex("+query.front().second+")");
    portage::UserinfoXML& userinfo_xml(GlobalUserinfoXML());
    const portage::Developers& devs(userinfo_xml.devs());
    const portage::Herds& herds(GlobalHerdsXML().herds());
    portage::Herds::const_iterator h;

    regexp.assign(query.front().second);
    query.clear();

    /* insert the user name of each developer
     * that matches the regex into our query object */
    for (h = herds.begin() ; h != herds.end() ; ++h)
        util::transform_if(h->begin(), h->end(), std::back_inserter(query),
            util::compose_f_gx(
                std::bind1st(util::regexMatch(), regexp),
                std::mem_fun_ref(&portage::Developer::user)),
                    std::mem_fun_ref(&portage::Developer::user));


    /* likewise for userinfo.xml, if used.  There may be
     * developers that match that aren't listed in herds.xml */
    if (not userinfo_xml.empty())
        util::transform_if(devs.begin(), devs.end(), std::back_inserter(query),
            util::compose_f_gx(
                std::bind1st(util::regexMatch(), regexp),
                std::mem_fun_ref(&portage::Developer::user)),
                    std::mem_fun_ref(&portage::Developer::user));

    if (query.empty())
    {
        results->add("Failed to find any developers matching '" +
                     regexp() + "'.");
        throw ActionException();
    }

    /* nuke dupes */
    std::sort(query.begin(), query.end(),
        util::compose_f_gx_hy(
            std::less<std::string>(),
            util::Second<Query::value_type>(),
            util::Second<Query::value_type>()));

    query.erase(std::unique(query.begin(), query.end(),
                    util::compose_f_gx_hy(
                        std::equal_to<std::string>(),
                        util::Second<Query::value_type>(),
                        util::Second<Query::value_type>())),
            query.end());
}

void
DevActionHandler::do_results(Query& query, QueryResults * const results)
{
    BacktraceContext c("DevActionHandler::do_results()");

    portage::HerdsXML& herds_xml(GlobalHerdsXML());
    portage::DevawayXML& devaway_xml(GlobalDevawayXML());
    portage::UserinfoXML& userinfo_xml(GlobalUserinfoXML());
    const portage::Developers& devs(userinfo_xml.devs());
    const portage::Herds& herds(herds_xml.herds());

    if (query.all() and options.quiet())
    {
        results->transform(query.begin(), query.end(),
                util::Second<QuerySpec>());
        return;
    }

    for (Query::iterator q = query.begin() ; q != query.end() ; ++q)
    {
        portage::Developer dev(q->second);
        herds_xml.fill_developer(dev);
        devaway_xml.fill_developer(dev);
        userinfo_xml.fill_developer(dev);

        if (dev.herds().empty() and (userinfo_xml.empty() or
            (devs.find(q->second) == devs.end())))
        {
            this->error() = true;
            results->add("Developer '" + q->second +
                         "' doesn't seem to belong to any herds.");

            if (options.iomethod() == "stream")
                throw ActionException();

            query.erase(q--);
        }
        else if (options.count())
            continue;
        else
        {
            const std::vector<std::string>& hvec(dev.herds());

            if (not options.quiet())
            {
                if (dev.name().empty())
                    results->add("Developer", q->second);
                else
                    results->add("Developer",
                            dev.name() + " (" + q->second + ")");

                results->add("Email", dev.email());
            }

            if (hvec.empty())
                results->add("Herds(0)", "none");
            else
            {
                if (options.verbose() and not options.quiet())
                {
                    results->add(util::sprintf("Herds(%d)", hvec.size()), "");

                    std::vector<std::string>::const_iterator i;
                    for (i = hvec.begin() ; i != hvec.end() ; ++i)
                    {
                        if (options.color())
                            results->add(color[blue] + (*i) + color[none]);
                        else
                            results->add(*i);

                        portage::Herds::const_iterator h = herds.find(*i);
                        if (not h->email().empty())
                            results->add(h->email());
                        if (not h->desc().empty())
                            results->add(h->desc());

                        if ((i+1) != hvec.end())
                            results->add_linebreak();
                    }
                }
                else
                    results->add(util::sprintf("Herds(%d)", hvec.size()), hvec);
            }

            /* display userinfo.xml stuff */
            if (not options.quiet())
            {
                if (not hvec.empty() and options.verbose()
                    and not userinfo_xml.empty())
                    results->add_linebreak();

#define DEFINED(x) ((not dev.x().empty() and (dev.x() != "undefined")))

                if (DEFINED(pgpkey))
                    results->add("PGP Key ID", dev.pgpkey());
                if (DEFINED(joined))
                {
                    const std::string elapsed(util::get_elapsed_yrs(dev.joined()));
                    if (elapsed.empty())
                        results->add("Joined Date", dev.joined());
                    else
                        results->add("Joined Date",
                            dev.joined() + " (" + elapsed + ")");
                }
                if (DEFINED(birthday))
                {
                    const std::string elapsed(util::get_elapsed_yrs(dev.birthday()));
                    if (elapsed.empty())
                        results->add("Birth Date", dev.birthday());
                    else
                        results->add("Birth Date", dev.birthday() + " ("+elapsed+")");
                }
                if (DEFINED(status))
                    results->add("Status", dev.status());
                if (DEFINED(role))
                    results->add("Roles", dev.role());
                if (DEFINED(location))
                    results->add("Location", dev.location());

#undef DEFINED

                if (dev.is_away() and not dev.awaymsg().empty())
                    results->add("Devaway",
                                 util::tidy_whitespace(dev.awaymsg()));

            }
            if ((q+1) != query.end())
                results->add_linebreak();
        }
    }
}

/* vim: set tw=80 sw=4 fdm=marker et : */
