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
    return "Get information about the given developer.";
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
DevActionHandler::operator()(const Query& query,
                              QueryResults * const results)
{
    Options& options(GlobalOptions());

    portage::herds_xml& herds_xml(GlobalHerdsXML());
    portage::devaway_xml& devaway_xml(GlobalDevawayXML());
    portage::userinfo_xml& userinfo_xml(GlobalUserinfoXML());

    if (not options.userinfoxml().empty())
        userinfo_xml.parse(options.userinfoxml());

    for (Query::const_iterator q = query.begin() ; q != query.end() ; ++q)
    {
        try
        {
            portage::Developer dev(q->second);
            herds_xml.fill_developer(dev);
            devaway_xml.fill_developer(dev);
            userinfo_xml.fill_developer(dev);

            if (dev.herds().empty() and (userinfo_xml.empty() or
                (userinfo_xml.devs().find(q->second) == userinfo_xml.devs().end())))
                throw ActionException();

            const std::vector<std::string>& herds(dev.herds());

            if (not options.quiet())
            {
                if (dev.name().empty())
                    results->add("Developer", q->second);
                else
                    results->add("Developer",
                            dev.name() + " (" + q->second + ")");

                results->add("Email", dev.email());
            }

            if (herds.empty())
            {
                if (not options.count())
                    results->add("Herds(0)", "none");
            }
            else
            {
                if (options.verbose() and not options.quiet())
                {
                    results->add(util::sprintf("Herds(%d)", herds.size()), "");

                    std::vector<std::string>::const_iterator i;
                    for (i = herds.begin() ; i != herds.end() ; ++i)
                    {
                        if (options.color())
                            results->add("", color[blue] + (*i) + color[none]);
                        else
                            results->add("", *i);

                        portage::Herds::const_iterator h =
                            herds_xml.herds().find(*i);
                        if (not h->email().empty())
                            results->add("", h->email());
                        if (not h->desc().empty())
                            results->add("", h->desc());

                        if ((i+1) != herds.end())
                            results->add("", "");
                    }
                }
                else if (not options.count())
                    results->add(util::sprintf("Herds(%d)", herds.size()), herds);

                size += herds.size();
            }

            /* display userinfo.xml stuff */
            if (not options.quiet())
            {
                if (not herds.empty() and options.verbose()
                    and not userinfo_xml.empty())
                    results->add("", "");

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
            }

#undef DEFINED

            if (dev.is_away() and not dev.awaymsg().empty())
                results->add("Devaway",
                             util::tidy_whitespace(dev.awaymsg()));

            if ((q+1) != query.end())
                results->add("", "");
        }
        catch (const ActionException)
        {
            results->add("", util::sprintf("Developer '%d' doesn't seem to exist.",
                    q->second.c_str()));
        }
    }
}

/* vim: set tw=80 sw=4 fdm=marker et : */
