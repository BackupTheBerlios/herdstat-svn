/*
 * herdstat -- src/herdstat.cc
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
#include <map>
#include <vector>
#include <algorithm>
#include <memory>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef HAVE_GETOPT_H
# include <getopt.h>
#endif

#include "herds.hh"
#include "xmlparser.hh"
#include "options.hh"
#include "util.hh"
#include "exceptions.hh"
#include "action_herd_handler.hh"
#include "action_pkg_handler.hh"
#include "action_dev_handler.hh"

static const char *short_opts = "H:hVvDdtpq";

#ifdef HAVE_GETOPT_LONG
static struct option long_opts[] =
{
    {"version",	    no_argument,	0,  'V'},
    {"help",	    no_argument,	0,  'h'},
    {"verbose",	    no_argument,	0,  'v'},
    {"quiet",	    no_argument,	0,  'q'},
    {"debug",	    no_argument,	0,  'D'},
    /* time how long it takes for XML parsing */
    {"timer",	    no_argument,	0,  't'},
    /* instead of displaying devs for a herd, display herds for a dev */
    {"dev",	    no_argument,	0,  'd'},
    /* specify the location of a local herds.xml */
    {"herdsxml",    required_argument,	0,  'H'},
    /* show package stats for the specified herds */
    {"package",	    no_argument,	0,  'p'},
    { 0, 0, 0, 0 }
};
#endif /* HAVE_GETOPT_LONG */

void
version()
{
    std::cout << PACKAGE << "-" << VERSION << std::endl;
}

void
usage()
{
    std::cerr
	<< "usage: " << PACKAGE << " [options] [args]" << std::endl
	<< "Use --help to see more detailed usage information." << std::endl;
}

void
help()
{
    std::cout
	<< "usage: " << PACKAGE << " [options] [args]" << std::endl

#ifdef HAVE_GETOPT_LONG
	<< " -h, --help            Display this help message." << std::endl
	<< " -V, --version         Display version information." << std::endl
	<< std::endl
	<< "Where [options] can be any of the following:" << std::endl
	<< " -p, --package         Look up packages by herd." << std::endl
	<< " -d, --dev             Look up herds by developer." << std::endl
	<< " -H, --herdsxml <file> Specify location of herds.xml." << std::endl
	<< " -v, --verbose         Display verbose output." << std::endl
	<< " -q, --quiet           Don't display labels and fancy colors. Use this"
	<< std::endl
	<< "                       option to pipe herdstat output to other programs"
	<< std::endl
	<< " -D, --debug           Display debugging messages." << std::endl
	<< " -t, --timer           Display elapsed time of XML parsing." << std::endl
	<< std::endl
	<< "Where [args] depends on the specified action:" << std::endl
	<< " default action        1 or more herds." << std::endl
	<< " -p, --package         1 or more herds." << std::endl
	<< " -d, --dev             1 or more developers." << std::endl

#else

	<< " -h              Display this help message." << std::endl
	<< " -V              Display version information." << std::endl
	<< std::endl
	<< "Where [options] can be any of the following:" << std::endl
	<< " -p              Look up packages by herd." << std::endl
	<< " -d              Look up herds by developer." << std::endl
	<< " -H <file>       Specify location of herds.xml." << std::endl
	<< " -v              Display verbose output." << std::endl
	<< " -q              Don't display labels and fancy colors. Use this"
	<< std::endl
	<< "                 option to pipe herdstat output to other programs"
	<< std::endl
	<< " -D              Display debugging messages." << std::endl
	<< " -t              Display elapsed time of XML parsing." << std::endl
	<< std::endl
	<< "Where [args] depends on the specified action:" << std::endl
	<< " default action  1 or more herds." << std::endl
	<< " -p              1 or more herds." << std::endl
	<< " -d              1 or more developers." << std::endl

#endif /* HAVE_GETOPT_LONG */

	<< std::endl
	<< PACKAGE << " respects the HERDS environment variable." << std::endl
	<< "Set it in your shell rcfile to permanently set the location of "
	<< "your herds.xml" << std::endl;
}

int
handle_opts(int argc, char **argv, options_T *opts,
    std::vector<std::string> *args)
{
    int key, opt_index = 0;

    while (true)
    {
#ifdef HAVE_GETOPT_LONG
	key = getopt_long(argc, argv, short_opts, long_opts, &opt_index);
#else
	key = getopt(argc, argv, short_opts);
#endif /* HAVE_GETOPT_LONG */

	if (key == -1)
	    break;

	switch (key)
	{
	    /* --dev */
	    case 'd':
		if (opts->action() != action_unspecified)
		    throw args_one_action_only_E();
		opts->set_action(action_dev);
		break;
	    /* --verbose */
	    case 'v':
		opts->set_verbose(true);
		break;
	    /* --quiet */
	    case 'q':
		opts->set_quiet(true);
		break;
	    /* --herdsxml */
	    case 'H':
		opts->set_herds_xml(optarg);
		break;
	    /* --debug */
	    case 'D':
		opts->set_timer(true);
		opts->set_debug(true);
		break;
	    /* --timer */
	    case 't':
		opts->set_timer(true);
		break;
	    /* --package */
	    case 'p':
		if (opts->action() != action_unspecified)
		    throw args_one_action_only_E();
		opts->set_action(action_pkg);
		break;
	    /* --version */
	    case 'V':
		throw args_version_E();
		break;
	    /* --help */
	    case 'h':
		throw args_help_E();
		break;
	    case 0:
		throw args_usage_E();
		break;
	    default:
		throw args_E();
		break;
	}
    }

    if (optind < argc)
    {
	while (optind < argc)
	    args->push_back(argv[optind++]);
    }
    else
	throw args_usage_E();

    return 0;
}

/* unary predicate for the remove_if() call below */
bool isNotAll(std::string &s) { return (s != "all"); }

int
main(int argc, char **argv)
{
    options_T options;
    util::timer_T timer;
    std::string fetched_location = util::sprintf("%s/%s/herds.xml",
	LOCALSTATEDIR, PACKAGE);

    /* HERDS */
    char *result = getenv("HERDS");
    if (result)
	options.set_herds_xml(result);
    else
    {
	/* if a fetched copy exists and is newer than 24hrs, use it.
	 * see the explanation below where we fetch as to why we go
	 * through the trouble of doing this. */
	struct stat s;
	if ((stat(fetched_location.c_str(), &s) == 0) and
	    ((time(NULL) - s.st_mtime) < 86400))
	    options.set_herds_xml(fetched_location);
    }

    /* PORTDIR */
    if ((result = getenv("PORTDIR")))
	options.set_portdir(result);

    /* try to determine current columns, otherwise use default */
    {
	char line[5];
	FILE *p = popen("stty size 2>/dev/null | cut -d' ' -f2", "r");
	if (p)
	{
	    if (std::fgets(line, sizeof(line) - 1, p) != NULL)
		options.set_maxcol(atoi(line));
	    pclose(p);
	}
    }

    try
    {
	util::color_map_T color;
	std::vector<std::string> nonopt_args;

	/* handle command line options */
	if (handle_opts(argc, argv, &options, &nonopt_args) != 0)
	    throw args_E();

	/* remove duplicates; also has the nice side advantage
	 * of sorting the output				*/
	std::sort(nonopt_args.begin(), nonopt_args.end());
	std::vector<std::string>::iterator pos =
	    std::unique(nonopt_args.begin(), nonopt_args.end());
	if (pos != nonopt_args.end())
	    nonopt_args.erase(pos);

	/* did the user specify the all target? */
	pos = std::find(nonopt_args.begin(), nonopt_args.end(), "all");
	if (pos != nonopt_args.end())
	{
	    /* yep, so remove everything but it */
	    pos = std::remove_if(nonopt_args.begin(), nonopt_args.end(),
		isNotAll);
	    if (pos != nonopt_args.end())
		nonopt_args.erase(pos);
	}

	/* every action handler needs to parse herds.xml for one reason
	 * or another, so let's get it over with. */
	std::auto_ptr<HerdsXMLHandler_T> handler(new HerdsXMLHandler_T());
	try
	{
	    /* NOTE: ideally we'd love to only fetch it when the timestamp and
	     * size are different, however, because the default location to
	     * download herds.xml is from ViewCVS, there is no Last-Modified
	     * header, meaning wget can't do timestamps :( so basically, if
	     * you don't want to have to download herds.xml every time you run
	     * herdstat, set HERDS to your local copy!
	     */

	    /* fetch herds.xml? */
	    if (options.herds_xml().find("://") != std::string::npos)
	    {
		if (not options.quiet())
		{
		    std::cout
			<< "Fetching herds.xml... (you can use a local "
			<< "copy by setting the HERDS variable)"
			<< std::endl << std::endl;
		}

		/* fetch it */
		if (util::fetch(options.herds_xml(), fetched_location) != 0)
		    throw fetch_E();

		options.set_herds_xml(fetched_location);
	    }

	    if (options.timer())
		timer.start();

	    /* parse herds.xml */
	    XMLParser_T parser(&(*handler));
	    parser.parse(options.herds_xml());

	    if (options.timer())
		timer.stop();
	}
	catch (const fetch_E)
	{
	    std::cerr << color[red] << "Error fetching " << options.herds_xml()
		<< color[none] << std::endl;
	    return EXIT_FAILURE;
	}
	catch (const XMLParser_E &e)
	{
	    std::cerr << color[red] << "Error parsing '" << e.file() << "': "
		<< e.error() <<  color[none] << std::endl;
	    return EXIT_FAILURE;
	}

	/* set default action */
	if (options.action() == action_unspecified)
	    options.set_action(action_herd);

	std::map<options_action_T, action_handler_T * > handlers;
	handlers.insert(std::make_pair(action_herd,
	    new action_herd_handler_T()));
	handlers.insert(std::make_pair(action_pkg,
	    new action_pkg_handler_T()));
	handlers.insert(std::make_pair(action_dev,
	    new action_dev_handler_T()));

	action_handler_T *action_handler = handlers[options.action()];
	if (action_handler)
	{
	    try
	    {
		if ((*action_handler)(&(*handler), nonopt_args) != EXIT_SUCCESS)
		    return EXIT_FAILURE;
	    }
	    catch (action_E)
	    {
		return EXIT_FAILURE;
	    }
	}
	else
	    throw args_unimplemented_E();

	if (options.timer())
	    throw timer_E();
    }
    catch (const timer_E)
    {
	std::cout << std::endl
	    << "Took " << timer.elapsed() << "ms to parse herds.xml" << std::endl;
    }
    catch (const args_help_E)
    {
	help();
	return EXIT_SUCCESS;
    }
    catch (const args_version_E)
    {
	version();
	return EXIT_SUCCESS;
    }
    catch (const args_usage_E)
    {
	usage();
	return EXIT_FAILURE;
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
