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
#include "action/herd.hh" /* for add_herd() */
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
DevActionHandler::operator()(Query& query, QueryResults * const results)
{
    portage::herds_xml& herds_xml(GlobalHerdsXML());
    portage::devaway_xml& devaway_xml(GlobalDevawayXML());
    portage::userinfo_xml& userinfo_xml(GlobalUserinfoXML());
    const portage::Developers& devs(userinfo_xml.devs());

    const portage::Herds& herds(herds_xml.herds());
    portage::Herds::const_iterator h;

    if (not options.userinfoxml().empty())
        userinfo_xml.parse(options.userinfoxml());

    if (query.all())
    {
        portage::Herd all_devs;
        for (h = herds.begin() ; h != herds.end() ; ++h)
            all_devs.insert(h->begin(), h->end());

        if (not userinfo_xml.empty())
            all_devs.insert(devs.begin(), devs.end());

        add_herd(all_devs, results);
        this->size() = all_devs.size();
        ActionHandler::operator()(query, results);
        return;
    }
    else if (options.regex())
    {
        regexp.assign(query.front().second);
        query.clear();

        /* insert the user name of each developer
         * that matches the regex into our query object */
        for (h = herds.begin() ; h != herds.end() ; ++h)
            transform_to_query_if(h->begin(), h->end(), query,
                std::bind1st(portage::UserRegexMatch<portage::Developer>(),
                regexp), portage::User());

        /* likewise for userinfo.xml, if used.  There may be
         * developers that match that aren't listed in herds.xml */
        if (not userinfo_xml.empty())
            transform_to_query_if(devs.begin(), devs.end(), query,
                std::bind1st(portage::UserRegexMatch<portage::Developer>(),
                regexp), portage::User());

        if (query.empty())
        {
            results->add(util::sprintf(
                "Failed to find any developers matching '%s'.", regexp().c_str()));
            throw ActionException();
        }

        /* remove any dupes */
        std::sort(query.begin(), query.end(), util::SecondLess());
        query.erase(std::unique(query.begin(), query.end(), util::SecondEqual()),
            query.end());
    }

    for (Query::iterator q = query.begin() ; q != query.end() ; ++q)
    {
        try
        {
            portage::Developer dev(q->second);
            herds_xml.fill_developer(dev);
            devaway_xml.fill_developer(dev);
            userinfo_xml.fill_developer(dev);

            if (dev.herds().empty() and (userinfo_xml.empty() or
                (devs.find(q->second) == devs.end())))
                throw ActionException();

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
            {
                if (not options.count())
                    results->add("Herds(0)", "none");
            }
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
                else if (not options.count())
                    results->add(util::sprintf("Herds(%d)", hvec.size()), hvec);

                this->size() += hvec.size();
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
        catch (const ActionException)
        {
            this->error() = true;
            results->add(util::sprintf("Developer '%s' doesn't seem to belong to any herds.",
                    q->second.c_str()));

            if (options.iomethod() == "stream")
                throw;
        }
    }

    ActionHandler::operator()(query, results);
}

/* vim: set tw=80 sw=4 fdm=marker et : */
