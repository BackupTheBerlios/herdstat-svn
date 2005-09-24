/*
 * herdstat -- src/action_herd_handler.cc
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

#include <iostream>
#include <fstream>
#include <memory>

#include <herdstat/util/string.hh>
#include <herdstat/util/algorithm.hh>

#include "action_herd_handler.hh"

using namespace portage;
using namespace util;

void
display_herd(const Herd& herd)
{
    formatter_T out;
    util::color_map_T color;

    Herd devs(herd);

    if (not options::quiet())
    {
        if (not herd.name().empty())
            out("Herd", herd.name());
        if (not herd.email().empty())
            out("Email", herd.email());
        if (not herd.desc().empty())
            out("Description", util::tidy_whitespace(herd.desc()));

        if (options::verbose())
            out(util::sprintf("Developers(%d)", herd.size()), "");
    }

    if (options::verbose() and not options::quiet())
    {
        const std::string user(util::current_user());

        for (Herd::const_iterator i = devs.begin() ; i != devs.end() ; ++i)
        {
            if ((i->user() == user) or not options::color())
                out("", i->email());
            else
                out("", color[blue] + i->email() + color[none]);

            if (not i->name().empty())
                out("", i->name());
            if (not i->role().empty())
                out("", i->role());
            if (not i->name().empty() or not i->role().empty())
                out.endl();
        }
    }

    if ((not options::verbose() and not options::quiet()) or
        (not options::verbose() and options::quiet() and not options::count()))
    {
        std::vector<std::string> dvec(devs);
        out(util::sprintf("Developers(%d)", dvec.size()), dvec);
    }
}

static void
display_herds(const Herds& herds)
{
    util::color_map_T color;
    formatter_T out;

    if (options::verbose() and not options::quiet())
    {
        out(util::sprintf("Herds(%d)", herds.size()), "");

        Herds::size_type n = 1;
        Herds::const_iterator h;
        for (h = herds.begin() ; h != herds.end() ; ++h)
        {
            if (options::color())
                out("", color[blue] + h->name() + color[none]);
            else
                out("", h->name());

            if (not h->desc().empty())
                out("", util::tidy_whitespace(h->desc()));

            if (not options::count() and n != herds.size())
                out.endl();
        }
    }
    else if (not options::count())
        out(util::sprintf("Herds(%d)", herds.size()), herds);
}

action_herd_handler_T::~action_herd_handler_T()
{
}

/*
 * Given a list of herds, display herd and developer
 * information.
 */

int
action_herd_handler_T::operator() (opts_type &opts)
{
    fetch_herdsxml();
    herdsxml.parse(options::herdsxml());
    const Herds& herds(herdsxml.herds());

    if (options::devaway())
    {
        fetch_devawayxml();
        devaway.parse(options::devawayxml());
    }

    /* set format attributes */
    output.set_maxlabel(options::all() ? 11 : 15);
    output.set_maxdata(options::maxcol() - output.maxlabel());
    if (options::devaway())
        output.set_devaway(devaway.keys());
    output.set_attrs();

    /* was the all target specified? */
    if (options::all())
    {
        display_herds(herds);
        size = herds.size();
        flush();
        return EXIT_SUCCESS;
    }
    else if (options::regex())
    {
        regexp.assign(opts.front(), options::eregex() ?
                Regex::extended|Regex::icase : Regex::icase);
        opts.clear();

        util::transform_if(herds.begin(), herds.end(), std::back_inserter(opts),
            std::bind1st(NameRegexMatch<Herd>(), &regexp), Name());
    }

    /* for each specified herd... */
    opts_type::iterator herd;
    opts_type::size_type n = 1;
    for (herd = opts.begin() ; herd != opts.end() ; ++herd, ++n)
    {
        /* does the herd exist? */
        Herds::const_iterator h = herds.find(*herd);
        if (h == herds.end())
        {
            std::cerr << "Herd '" << *herd << "' doesn't seem to exist."
                << std::endl;

            /* if the user specified more than one herd, then just print
             * the error and keep going; otherwise, we want to exit with
             * an error code */
            if (opts.size() > 1)
            {
                std::cerr << std::endl;
                continue;
            }
            else
                throw HerdException();
        }

        display_herd(*h);
        size += h->size();

        /* only skip a line if we're not displaying the last one */
        if (not options::count() and n != opts.size())
            output.endl();
    }

    flush();
    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
