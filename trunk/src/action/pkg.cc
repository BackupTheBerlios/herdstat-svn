/*
 * herdstat -- src/action/pkg.cc
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

#include <herdstat/util/progress/spinner.hh>

#include "common.hh"
#include "action/meta.hh"
#include "action/pkg.hh"

using namespace herdstat;
using namespace gui;

PkgActionHandler::PkgActionHandler()
    : metacache(), with(), herds_xml(GlobalHerdsXML())
{
}

const char * const
PkgActionHandler::id() const
{
    return "pkg";
}

const char * const
PkgActionHandler::desc() const
{
    return "Find packages maintained by the given herd/developer.";
}

const char * const
PkgActionHandler::usage() const
{
    return "pkg <herd(s)|developer(s)>";
}

Tab *
PkgActionHandler::createTab(WidgetFactory *widgetFactory)
{
    Tab *tab = widgetFactory->createTab();
    tab->set_title(this->id());
    return tab;
}

bool
PkgActionHandler::metadata_matches(const portage::Metadata& meta,
                                   const std::string& criteria)
{
    BacktraceContext c("PkgActionHandler::metadata_matches()");

    const portage::Herds& herds(meta.herds());
    const portage::Developers& devs(meta.devs());

    if (options.dev())
    {
        if ((options.regex() and (devs.find(regexp) != devs.end()) and
            (with.empty() or (herds.find(with) != herds.end()) or
            (with() == "no-herd" and herds.empty()))) or
            (not options.regex() and (devs.find(criteria) != devs.end()) and
            (with.empty() or (herds.find(with) != herds.end()) or
            (with() == "no-herd" and herds.empty()))))
            return true;
    }
    else
    {
        if ((options.regex() and (herds.find(regexp) != herds.end())) or
            (not options.regex() and (herds.find(criteria) != herds.end())) or
            (criteria == "no-herd" and herds.empty()))
        {
            if (with.empty())
                return true;
            else
            {
                /* --no-maintainer was specified.  It's true if there
                 * are a) no maintainers, or b) the herd is listed as
                 * a maintainer. */
                if (with() == "none")
                {
                    if (devs.empty() or ((devs.size() == 1) and
                        (devs.find(criteria) != devs.end())))
                        return true;
                }
                else if (devs.find(with) != devs.end())
                    return true;
            }
        }
    }

    return false;
}

void
PkgActionHandler::add_matches(QueryResults * const results)
{
    const portage::Herds& herds(herds_xml.herds());
    portage::Herds::const_iterator h;

    for (matches_type::const_iterator m = matches.begin() ;
            m != matches.end() ; )
    {
        const std::string& criteria(m->first);
        const std::set<portage::Metadata>& metadatas(*m->second);

        if (not options.quiet())
        {
            if (options.regex())
                results->add("Regex", criteria);
            else if (options.dev())
            {
                portage::Developer dev(criteria);
                herds_xml.fill_developer(dev);

                if (dev.name().empty())
                    results->add("Developer", criteria);
                else
                    results->add("Developer",
                            dev.name() + " (" + criteria + ")");

                results->add("Email", dev.email());
            }
            else
            {
                results->add("Herd", criteria);

                if ((h = herds.find(criteria)) != herds.end())
                {
                    if (not h->email().empty())
                        results->add("Email", h->email());
                    if (not h->desc().empty())
                        results->add("Description", h->desc());
                }
            }

            if (metadatas.empty())
                results->add("Packages(0)", "none");
            else if (options.verbose() and options.color())
                results->add(util::sprintf("Packages(%d)", metadatas.size()),
                        color[blue] + metadatas.begin()->pkg() + color[none]);
            else
                results->add(util::sprintf("Packages(%d)", metadatas.size()),
                        metadatas.begin()->pkg());
        }
        else if (not metadatas.empty() and not options.count())
            results->add(metadatas.begin()->pkg());

        std::set<portage::Metadata>::const_iterator i;
        for (i = ++metadatas.begin() ; i != metadatas.end() ; )
        {
            std::string longdesc;

            if (not i->longdesc().empty())
                longdesc = util::tidy_whitespace(i->longdesc());

            if ((options.verbose() and not options.quiet()) and
                    not longdesc.empty())
            {
                if (results->size() > 1 and
                    results->back() != QueryResults::value_type("", ""))
                    results->add_linebreak();

                if (options.color())
                    results->add(color[blue] + i->pkg() + color[none]);
                else
                    results->add(i->pkg());

                results->add(longdesc);

                if (++i != metadatas.end())
                    results->add_linebreak();

                continue;
            }
            else if (options.verbose() and not options.quiet())
            {
                if (options.color())
                    results->add(color[blue] + i->pkg() + color[none]);
                else
                    results->add(i->pkg());
            }
            else if (not options.count())
                results->add(i->pkg());

            ++i;
        }

        if (++m != matches.end())
            results->add_linebreak();
    }
}

void
PkgActionHandler::do_init(Query& query, QueryResults * const results)
{
    ActionHandler::do_init(query, results);

    if (options.spinner())
    {
//        assert(spinner == NULL);
        if (not spinner)
            spinner = new util::Spinner();
        if (not spinner->started())
            spinner->start(1000, "Performing query");
        metacache.set_spinner(spinner);
    }

    this->size() = 0;

    /* setup with regex */
    with.assign((options.dev() ? options.with_herd() :
                                 options.with_dev()),
                                 util::Regex::icase);

    /* load metadata cache */
    if (metacache.is_valid())
        metacache.load();
    else
    {
        metacache.fill();
        metacache.dump();
    }
}

void
PkgActionHandler::do_regex(Query& query, QueryResults * const results)
{
}

void
PkgActionHandler::do_results(Query& query, QueryResults * const results)
{
    MetadataCache::const_iterator m;
    for (m = metacache.begin() ; m != metacache.end() ; ++m)
    {
        if (spinner)
            ++*spinner;

        for (Query::const_iterator q = query.begin() ; q != query.end() ; ++q)
        {
            const std::string& criteria(q->second);
    
            if (options.regex())
                regexp.assign(criteria);
            
            if (metadata_matches(*m, q->second))
            {
                matches_type::iterator i = matches.find(criteria);

                if (i == matches.end())
                    i = matches.insert(std::make_pair(criteria,
                            new std::set<portage::Metadata>())).first;
                    
                if (i->second->insert(*m).second)
                    this->size()++;
            }
        }
    }

    /* add error messages for the queries not found */
    if (not options.quiet() and (query.size() != matches.size()))
    {
        for (Query::const_iterator q = query.begin() ; q != query.end() ; ++q)
        {
            if (matches.find(q->second) == matches.end())
            {
                std::ostringstream error;
                error << "Failed to find any packages maintained by "
                    << (options.dev() ? "developer" : "herd")
                    << " '" << q->second << "'";

                if (with.empty())
                    error << ".";
                else
                    error << " with "
                        << (options.dev() ? "herd" : "developer") << " '"
                        << with() << "'.";

                results->add(error.str());
            }
        }

        if (matches.empty())
        {
            if (spinner and spinner->started())
                spinner->stop();
            throw ActionException();
        }
        else
            results->add_linebreak();
    }

    if (not options.count() and not options.meta())
        add_matches(results);
    else if (not options.count() and options.meta())
    {
        /* disable stuff we've handled already */
        const bool re(options.regex());
        const bool ere(options.eregex());
        const bool t(options.timer());

        options.set_regex(false);
        options.set_eregex(false);
        options.set_timer(false);

        Query q;
        for (matches_type::iterator i = matches.begin() ;
                i != matches.end() ; ++i)
        {
            std::transform(i->second->begin(), i->second->end(),
                std::back_inserter(q),
                std::mem_fun_ref(&portage::Metadata::pkg));
        }

        MetaActionHandler mh;
        mh.do_results(q, results);

        options.set_regex(re);
        options.set_eregex(ere);
        options.set_timer(t);
    }
}

void
PkgActionHandler::do_cleanup(QueryResults * const results)
{
    ActionHandler::do_cleanup(results);

    std::for_each(matches.begin(), matches.end(),
        util::compose_f_gx(
            util::DeleteAndNullify<std::set<portage::Metadata> >(),
            util::Second<matches_type::value_type>()));

    matches.clear();
}

/* vim: set tw=80 sw=4 fdm=marker et : */
