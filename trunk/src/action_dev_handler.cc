/*
 * herdstat -- src/action_dev_handler.cc
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

#include <iostream>
#include <herdstat/util/misc.hh>
#include <herdstat/util/string.hh>
#include <herdstat/util/algorithm.hh>

#include "exceptions.hh"
#include "action_herd_handler.hh"   /* for display_herd() */
#include "action_dev_handler.hh"

using namespace portage;
using namespace util;

action_dev_handler_T::action_dev_handler_T()
{
}

action_dev_handler_T::~action_dev_handler_T()
{
}

/*
 * Display data for the specified developer.
 */

void
action_dev_handler_T::display(const std::string &d)
{
    if (not options::fields().empty() and options::count())
        return;

    Developer dev(d);

    /* fill Developer object with data from each respective XML file */
    herdsxml.fill_developer(dev);
    devaway.fill_developer(dev);
    userinfo.fill_developer(dev);
    
    /* bail if userinfo.xml wasn't used and dev is not in any herds */
    if (dev.herds().empty() and (userinfo.empty() or
        (userinfo.devs().find(d) == userinfo.devs().end())))
        throw DevException();

    const std::vector<std::string>& herds(dev.herds());

    /* temporarily disable quiet */
    if (not options::fields().empty())
    {
        options::set_quiet(false);
        output.set_quiet(false);
    }

    if (not options::quiet())
    {
        if (dev.name().empty())
            output("Developer", d);
        else
            output("Developer", dev.name() + " (" + d + ")");
        
        output("Email", dev.email());
    }

    if (herds.empty())
    {
        if (not options::count())
            output("Herds(0)", "none");
    }
    else
    {
        if (options::verbose() and not options::quiet())
        {
            output(util::sprintf("Herds(%d)", herds.size()), "");

            std::vector<std::string>::const_iterator i;
            std::vector<std::string>::size_type nh = 1;
            for (i = herds.begin() ; i != herds.end() ; ++i, ++nh)
            {
                /* display herd */
                if (options::color())
                    output("", color[blue] + (*i) + color[none]);
                else
                    output("", *i);
                        
                /* display herd info */
                Herds::const_iterator h = herdsxml.herds().find(*i);
                if (not h->email().empty())
                    output("", h->email());
                if (not h->desc().empty())
                    output("", h->desc());

                if (nh != herds.size())
                    output.endl();
            }
        }
        else if (not options::count())
            output(util::sprintf("Herds(%d)", herds.size()), herds);

        if (options::fields().empty())
            size += herds.size();
    }

    /* display userinfo.xml stuff */
    if (not options::quiet())
    {
        if (not herds.empty() and options::verbose() and not userinfo.empty())
            output.endl();

#define DEFINED(x) ((not dev.x().empty() and (dev.x() != "undefined")))

        if (DEFINED(pgpkey))
            output("PGP Key ID", dev.pgpkey());
        if (DEFINED(joined))
        {
            const std::string elapsed(util::get_elapsed_yrs(dev.joined()));
            if (elapsed.empty())
                output("Joined Date", dev.joined());
            else
                output("Joined Date", dev.joined() + " ("+elapsed+")");
        }
        if (DEFINED(birthday))
        {
            const std::string elapsed(util::get_elapsed_yrs(dev.birthday()));
            if (elapsed.empty())
                output("Birth Date", dev.birthday());
            else
                output("Birth Date", dev.birthday() + " ("+elapsed+")");
        }
        if (DEFINED(status))
            output("Status", dev.status());
        if (DEFINED(role))
            output("Roles", dev.role());
        if (DEFINED(location))
            output("Location", dev.location());

#undef DEFINED

        if (dev.is_away() and not dev.awaymsg().empty())
            output("Devaway", util::tidy_whitespace(dev.awaymsg()));
    }
}

/* pointer to Developer const mem. function that returns a const std::string& */
typedef const std::string& (Developer::*developer_mfp)(void) const;

/* for mapping strings to Developer member functions */
typedef std::map<std::string, developer_mfp> dfmap;

template <typename OutputIterator>
void
transform_fields_into_matches(Developers::const_iterator first,
        Developers::const_iterator last, OutputIterator result,
        const fields_type& fields, const dfmap& fm)
{
    Regex queryre;
    const int cflags(options::eregex() ?
            Regex::extended|Regex::icase : Regex::icase);

    for (; first != last ; ++first)
    {
        fields_type::const_iterator f;
        for (f = fields.begin() ; f != fields.end() ; ++f)
        {
            /* check if field is valid */
            dfmap::const_iterator i = fm.find(f->first);
            if (i == fm.end())
                throw InvalidField(f->first);

            /* it's valid, so compile regex */
            queryre.assign(f->second, cflags);

            /* compare criteria against the return value of the
             * Developer member function mapped to this field. */
            const Developer& dev(*first);
            if (queryre != (dev.*(i->second))())
                break;

            /* we're on the last field, meaning all fields that came before
             * it also matched, so save it finally. */
            if ((f+1) == fields.end())
                *result++ = first->user();
        }
    }
}

/*
 * Given a list of developers, display all herds that
 * each developer belongs to.
 */

int
action_dev_handler_T::operator() (opts_type &opts)
{
    fetch_herdsxml();
    herdsxml.parse(options::herdsxml());

    const Herds& herds(herdsxml.herds());
    Herds::const_iterator h;

    if (options::devaway())
    {
        fetch_devawayxml();
        devaway.parse(options::devawayxml());
    }

    if (not options::userinfoxml().empty())
        userinfo.parse(options::userinfoxml());

    /* set format attributes */
    output.set_maxlabel(options::all() ? 16 : 12);
    output.set_maxdata(options::maxcol() - output.maxlabel());
    /* set away devs (for use in marking them when they occur in output) */
    if (options::devaway())
        output.set_devaway(devaway.keys());
    output.set_attrs();

    /* all target? */
    if (options::all())
    {
        Herd all_devs;

        /* for each herd... */
        for (h = herds.begin() ; h != herds.end() ; ++h)
            all_devs.insert(h->begin(), h->end());

        /* insert those that exist in userinfo.xml but not herds.xml */
        if (not userinfo.empty())
            all_devs.insert(userinfo.devs().begin(), userinfo.devs().end());

        display_herd(all_devs);
        size = all_devs.size();
        flush();
        return EXIT_SUCCESS;
    }
    else if (options::fields().empty() and options::regex())
    {
        const std::string re(opts.front());
        opts.clear();

        regexp.assign(re, options::eregex() ?
                Regex::extended|Regex::icase : Regex::icase);

        /* loop through herds searching for devs who's username
         * matches the regular expression, inserting those that do into opts */
        for (h = herds.begin() ; h != herds.end() ; ++h)
            util::transform_if(h->begin(), h->end(), std::back_inserter(opts),
                std::bind1st(UserRegexMatch<Developer>(), &regexp), User());

        /* also add those in userinfo.xml - dupes will be unique'd out below */
        if (not userinfo.empty())
            util::transform_if(userinfo.devs().begin(), userinfo.devs().end(),
                std::back_inserter(opts), std::bind1st(UserRegexMatch<Developer>(),
                &regexp), User());

        if (opts.empty())
        {
            std::cerr << "Failed to find any developers matching '" << re
                << "'." << std::endl;
            return EXIT_FAILURE;
        }

        /* remove any dupes */
        std::sort(opts.begin(), opts.end());
        opts.erase(std::unique(opts.begin(), opts.end()), opts.end());
    }
    else if (not options::fields().empty())
    {
        if (userinfo.empty())
        {
            std::cerr << "--field only makes sense when used with userinfo.xml."
                << std::endl;
            return EXIT_FAILURE;
        }

        try
        {
            dfmap fm;

            /* insert valid --field values and their
             * corresponding comparison function */
            fm.insert(std::make_pair("user",        &Developer::user));
            fm.insert(std::make_pair("name",        &Developer::name));
            fm.insert(std::make_pair("location",    &Developer::location));
            fm.insert(std::make_pair("status",      &Developer::status));
            fm.insert(std::make_pair("joined",      &Developer::joined));
            fm.insert(std::make_pair("birthday",    &Developer::birthday));

            /* for each user in userinfo.xml, insert
             * the developer's username into the opts
             * vector if the specified criteria for
             * each --field is met. */
            transform_fields_into_matches(userinfo.devs().begin(),
                userinfo.devs().end(), std::back_inserter(opts), options::fields(), fm);
        }
        catch (const InvalidField& e)
        {
            std::cerr << "Unrecognized field '" << e.what() << "'." << std::endl
                << "Possibles are: name,user,location,status,birthday,joined."
                << std::endl;
            return EXIT_FAILURE;
        }

        size = opts.size();
    }

    /* for each specified dev... */
    for (opts_type::iterator i = opts.begin() ; i != opts.end() ;  ++i)
    {
        try
        {
            display(*i);
        }
        catch (const DevException)
        {
            std::cerr << "Developer '" << *i << "' doesn't seem to "
                << "exist." << std::endl;

            if (opts.size() == 1)
                return EXIT_FAILURE;
            else
                continue;
        }

        /* skip a line if we're not displaying the last one */
        if (not options::count() and ((i+1) != opts.end()))
            output.endl();
    }

    flush();
    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
