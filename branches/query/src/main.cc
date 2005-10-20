/*
 * herdstat -- src/main.cc
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
#include <map>
#include <cstring>

#include <herdstat/exceptions.hh>

#ifdef HAVE_GETOPT_H
# include <getopt.h>
#endif

#include "query.hh"
#include "action/dev.hh"
#include "action/herd.hh"
#include "input/readline.hh"
#include "output/stream.hh"
#include "options.hh"
#include "exceptions.hh"

#define HERDSTATRC_GLOBAL   SYSCONFDIR"/herdstatrc"
#define HERDSTATRC_LOCAL    /*HOME*/"/.herdstatrc"

using namespace herdstat;

static const char *short_opts = "H:o:hVvDdtpqFcnmwNErfaA:L:C:U:TX:";

#ifdef HAVE_GETOPT_LONG
static struct option long_opts[] =
{
    {"version",     no_argument,        0,  'V'},
    {"help",        no_argument,        0,  'h'},
    {"verbose",     no_argument,        0,  'v'},
    {"quiet",       no_argument,        0,  'q'},
    {"debug",       no_argument,        0,  'D'},
    {"count",       no_argument,        0,  'c'},
    {"nocolor",     no_argument,        0,  'n'},
    {"no-herd",     no_argument,        0,  '\n'},
    {"with-herd",   required_argument,  0,  '\v'},
    {"no-maintainer", no_argument,      0,  '\r'},
    {"with-maintainer", required_argument,0,'\t'},
    /* force a fetch of herds.xml */
    {"fetch",       no_argument,        0,  'F'},
    /* time how long it takes for XML parsing */
    {"timer",       no_argument,        0,  't'},
    /* instead of displaying devs for a herd, display herds for a dev */
    {"dev",         no_argument,        0,  'd'},
    /* specify the location of a local herds.xml */
    {"herdsxml",    required_argument,  0,  'H'},
    /* specify the location of devaway.xml. */
    {"devaway",     required_argument,  0,  'A'},
    /* specify the location of userinfo.xml */
    {"userinfo",    required_argument,  0,  'U'},
    /* specify Gentoo CVS directory */
    {"gentoo-cvs",  required_argument,  0,  'C'},
    /* specify local statedir */
    {"localstatedir", required_argument,0,  'L'},
    /* show package stats for the specified herds */
    {"package",     no_argument,        0,  'p'},
    /* display package metadata information */
    {"metadata",    no_argument,        0,  'm'},
    {"which",       no_argument,        0,  'w'},
    {"versions",    no_argument,        0,  '\b'},
    {"find",        no_argument,        0,  'f'},
    {"away",        no_argument,        0,  'a'},
    /* specify a file to write the output to */
    {"outfile",     required_argument,  0,  'o'},
    {"no-overlay",  no_argument,        0,  'N'},
    {"regex",       no_argument,        0,  'r'},
    {"extended",    no_argument,        0,  'E'},
    {"qa",          no_argument,        0,  '\a'},
    {"nometacache",  no_argument,       0,  '\f'},
    {"TEST",        no_argument,        0,  'T'},
    {"field",       required_argument,  0,  'X'},
    { 0, 0, 0, 0 }
};
#endif /* HAVE_GETOPT_LONG */

static void
version()
{
    std::cout << PACKAGE << "-" << VERSION
        << " (built: " << __DATE__ << " " << __TIME__ << ")" << std::endl;
    std::cout << "Options:";
#ifdef FETCH_METHOD_CURL
    std::cout << " +curl";
#endif
#ifdef INPUT_METHOD_READLINE
    std::cout << " +readline";
#endif
#ifdef HAVE_NCURSES
    std::cout << " +ncurses";
#endif
    
    std::cout << std::endl;
}
    
static void
usage()
{
    std::cerr
        << "usage: " << PACKAGE << " [options] [args]" << std::endl
        << "Use --help to see more detailed usage information." << std::endl;
}

static void
help()
{
    version();
    std::cout << std::endl
        << "usage: " << PACKAGE << " [options] [args]" << std::endl;
}

static bool
handle_opts(int argc, char **argv,
        Query *q, std::vector<std::string> *args)
{

    return true;
}

std::string::size_type getcols(); // defined in getcols.cc

int
main(int argc, char **argv)
{
    options opts;

    const bool test = ((argc > 1) and
                      ((std::strcmp(argv[1], "--TEST") == 0) or
                       (std::strcmp(argv[1], "-T") == 0)));

    options::set_maxcol((test ? 80 : util::getcols()));

    try
    {
        Query q;
        std::vector<std::string> nonopt_args;

        /* handle command line options */
        if (not handle_opts(argc, argv, &q, &nonopt_args))
            throw argsException();

        if (options::iomethod() == IOMethodUnspecified)
            options::set_iomethod(IOMethodStream);

        /* setup I/O handlers */
        std::map<IOMethod, IOHandler *> iohandlers;
        iohandlers[IOMethodReadline] = new ReadlineIOHandler();
        iohandlers[IOMethodStream]   = new StreamIOHandler();
        iohandlers[IOMethodGtk]      = new GtkIOHandler();

        IOHandler *iohandler = iohandlers[options::iomethod()];
        if (not iohandler)
            throw IOMethodUnimplemented();

        /* setup action handlers */
        std::map<ActionMethod, ActionHandler *> ahandlers;
        ahandlers[ActionHerd] = new HerdActionHandler();
        ahandlers[ActionDev]  = new DevActionHandler();

        bool loop = (options::iomethod() != IOMethodStream);

        /* main loop */
        do
        {
            Query query;
            QueryResults results;

            /* handle input */
            if (options::iomethod() == IOMethodStream)
            {
                /* we've already filled the query object when 
                 * parsing the command line options, so use it
                 * instead of calling the iohandler. */
                query = q;
            }
            else if (not (*iohandler)(&query))
                break;

            ActionHandler *handler = ahandlers[query.action()];
            if (handler)
            {
                try
                {
                    /* perform action */
                    (*handler)(query, &results);
                }
                catch (const ActionException)
                {
                    return EXIT_FAILURE;   
                }
            }
            else
                throw ActionUnimplemented();

            /* display results */
            (*iohandler)(results);

        } while (loop);
    }
    catch (const argsHelp)
    {
        help();
        return EXIT_SUCCESS;
    }
    catch (const argsVersion)
    {
        version();
        return EXIT_SUCCESS;
    }
    catch (const argsUsage)
    {
        usage();
        return EXIT_FAILURE;
    }
    catch (const argsException)
    {
        usage();
        return EXIT_FAILURE;
    }
    catch (const Exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (const BaseException& e)
    {
        std::cerr << "Unhandled exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
