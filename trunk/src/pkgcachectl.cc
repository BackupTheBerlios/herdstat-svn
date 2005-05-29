/*
 * herdstat -- src/pkgcachectl.cc
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
#include "formatter.hh"
#include "pkgcache.hh"

#ifdef HAVE_GETOPT_H
# include <getopt.h>
#endif

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
        << "usage: pkgcachectl [action]" << std::endl
        << " -V, --version         Display version of herdstat this was shipped with." << std::endl
        << " -d, --dump            Dump package cache to stdout." << std::endl
        << " -c, --clear           Clear package cache." << std::endl
        << " -s, --summary         Show summary of information about the package cache." << std::endl;
}

int
handle_opts(int argc, char **argv, util::string *action)
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
                throw args_version_E();
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
                throw args_E();
                break;
            default:
                throw args_E();
                break;
        }
    }

    if (optind < argc)
        throw args_usage_E();

    return 0;
}

static void
doaction(pkgCache_T &pkgcache, const util::string &action)
{
    if (action == "dump")
    {
        if (pkgcache.empty())
            std::cerr << "Package cache is empty." << std::endl;
        else
            pkgcache.dump(&std::cout);
    }
    else if (action == "clear")
    {
        pkgcache.clear();
        pkgcache.dump();
    }
    else if (action == "summary")
    {
        formatter_T out;
        out.set_maxlabel(15);
        out.set_maxdata(optget("maxcol", std::size_t) - out.maxlabel());
        out.set_attrs();

        out("Size", util::sprintf("%d", pkgcache.size()));

        if (pkgcache.size() > 1)
        {
            pkgcache.sort_oldest_to_newest();
            out("Oldest query", util::format_date(pkgcache.front()->date, "%s")
                + " (" + util::format_date(pkgcache.front()->date) + ")");
            out("Newest query", util::format_date(pkgcache.back()->date, "%s")
                + " (" + util::format_date(pkgcache.back()->date) + ")");
        }

        out("Query strings", pkgcache.queries());
    }
    else
        throw args_E();
}

int
main(int argc, char **argv)
{
    util::color_map_T color;
    formatter_T out;
    out.set_colors(true);
    out.set_labelcolor(color[green]);
    out.set_highlightcolor(color[yellow]);
    out.add_highlight(util::current_user());

    try
    {
        util::string action;

        if (handle_opts(argc, argv, &action) != 0)
            throw args_E();

        pkgCache_T pkgcache;
        pkgcache.load();

        doaction(pkgcache, action);
        out.flush(std::cout);
    }
    catch (const args_version_E)
    {
        version();
        return EXIT_SUCCESS;
    }
    catch (const args_E)
    {
        usage();
        return EXIT_FAILURE;
    }
    catch (const herdstat_base_E &e)
    {
        std::cerr << "Unhandled exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
