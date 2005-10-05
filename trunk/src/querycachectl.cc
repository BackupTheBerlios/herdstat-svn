/*
 * herdstat -- src/querycachectl.cc
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
#include <herdstat/exceptions.hh>
#include <herdstat/util/string.hh>

#include "rc.hh"
#include "formatter.hh"
#include "querycache.hh"

#ifdef HAVE_GETOPT_H
# include <getopt.h>
#endif

using namespace herdstat;

static const char *short_options = "dscV";

#ifdef HAVE_GETOPT_LONG
static struct option long_options[] =
{
    {"version",     no_argument,        0,  'V'},
    {"dump",        no_argument,        0,  'd'},
    {"clear",       no_argument,        0,  'c'},
    {"summary",     no_argument,        0,  's'},
    { 0, 0, 0, 0 }
};
#endif /* HAVE_GETOPT_LONG */

static void version() { std::cout << PACKAGE << "-" << VERSION << std::endl; }
static void usage()
{
    std::cout
        << "usage: querycachectl [action]" << std::endl
        << " -V, --version         Display version of herdstat this was shipped with." << std::endl
        << " -d, --dump            Dump package cache to stdout." << std::endl
        << " -c, --clear           Clear package cache." << std::endl
        << " -s, --summary         Show summary of information about the package cache." << std::endl;
}

int
handle_opts(int argc, char **argv, std::string *action)
{
    int key, opt_index = 0;

    while (true)
    {
#ifdef HAVE_GETOPT_LONG
        key = getopt_long(argc, argv, short_options, long_options, &opt_index);
#else
        key = getopt(argc, argv, short_options);
#endif /* HAVE_GETOPT_LONG */

        if (key == -1)
            break;

        switch (key)
        {
            case 'V':
                throw argsVersion();
                break;
            case 'd':
                (*action)="dump";
                break;
            case 'c':
                (*action)="clear";
                break;
            case 's':
                (*action)="summary";
                break;
            case 0:
                throw argsException();
                break;
            default:
                throw argsException();
                break;
        }
    }

    if (optind < argc)
        throw argsUsage();

    return 0;
}

static void
doaction(querycache_T &querycache, const std::string &action)
{
    if (action == "dump")
    {
        if (querycache.empty())
            std::cerr << "Package query cache is empty." << std::endl;
        else
            querycache.dump(std::cout);
    }
    else if (action == "clear")
    {
        querycache.clear();
        querycache.dump();
    }
    else if (action == "summary")
    {
        formatter_T out;
        out.set_maxlabel(15);
        out.set_maxdata(options::maxcol() - out.maxlabel());
        out.set_attrs();

        out("Size", util::sprintf("%d", querycache.size()));

        if (querycache.size() > 1)
        {
            querycache.sort();

            std::string oldest(querycache.front().query);
            std::string newest(querycache.back().query);

            if (not querycache.front().with.empty())
                oldest += "/" + querycache.front().with;
            if (not querycache.back().with.empty())
                newest += "/" + querycache.back().with;

            oldest += " " + util::format_date(querycache.front().date, "%s")
                + " (" + util::format_date(querycache.front().date) + ")";
            newest += " " + util::format_date(querycache.back().date, "%s")
                + " (" + util::format_date(querycache.back().date) + ")";

            out("Oldest query", oldest);
            out("Newest query", newest);
        }

        out("Query strings", querycache.queries());
    }
    else
        throw argsException();
}

int
main(int argc, char **argv)
{
    util::ColorMap color;
    formatter_T out;
    out.set_colors(true);
    out.set_labelcolor(color[green]);
    out.set_highlightcolor(color[yellow]);
    out.add_highlight(util::current_user());

    try
    {
        std::string action;

        { rc_T rc; }

        if (handle_opts(argc, argv, &action) != 0)
            throw argsException();

        querycache_T querycache;
        querycache.load();

        doaction(querycache, action);
        out.flush(std::cout);
    }
    catch (const argsVersion)
    {
        version();
        return EXIT_SUCCESS;
    }
    catch (const argsException)
    {
        usage();
        return EXIT_FAILURE;
    }
    catch (const BaseException &e)
    {
        std::cerr << "Unhandled exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
