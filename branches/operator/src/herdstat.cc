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
#include <cerrno>
#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef HAVE_GETOPT_H
# include <getopt.h>
#endif

#include "herds.hh"
#include "formatter.hh"
#include "xmlparser.hh"
#include "options.hh"
#include "util.hh"
#include "exceptions.hh"
#include "herds_xml_handler.hh"
#include "action_herd_handler.hh"
#include "action_pkg_handler.hh"
#include "action_dev_handler.hh"
#include "action_stats_handler.hh"

static const std::string default_herdsxml =
    "http://www.gentoo.org/cgi-bin/viewcvs.cgi/misc/herds.xml?rev=HEAD;cvsroot=gentoo;content-type=text/plain";

static const char *short_opts = "H:o:hVvDdtpqfc";

#ifdef HAVE_GETOPT_LONG
static struct option long_opts[] =
{
    {"version",	    no_argument,	0,  'V'},
    {"help",	    no_argument,	0,  'h'},
    {"verbose",	    no_argument,	0,  'v'},
    {"quiet",	    no_argument,	0,  'q'},
    {"debug",	    no_argument,	0,  'D'},
    {"count",	    no_argument,	0,  'c'},
    /* force a fetch of herds.xml */
    {"fetch",	    no_argument,	0,  'f'},
    /* time how long it takes for XML parsing */
    {"timer",	    no_argument,	0,  't'},
    /* instead of displaying devs for a herd, display herds for a dev */
    {"dev",	    no_argument,	0,  'd'},
    /* specify the location of a local herds.xml */
    {"herdsxml",    required_argument,	0,  'H'},
    /* show package stats for the specified herds */
    {"package",	    no_argument,	0,  'p'},
    /* specify a file to write the output to */
    {"outfile",	    required_argument,	0,  'o'},
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
	<< " -o, --outfile  <file> Send output to the specified file" << std::endl
	<< "                       instead of stdout." << std::endl
	<< " -f, --fetch           Force a fetch of herds.xml." << std::endl
	<< " -v, --verbose         Display verbose output." << std::endl
	<< " -q, --quiet           Don't display labels and fancy colors. Use this"
	<< std::endl
	<< "                       option to pipe herdstat output to other programs"
	<< std::endl
	<< " -D, --debug           Display debugging messages." << std::endl
	<< " -t, --timer           Display elapsed time of XML parsing." << std::endl
	<< " -c, --count           Instead of displaying data items, display the" << std::endl
	<< "                       number of items." << std::endl
	<< " -c, --count           Display the number of items instead of the" << std::endl
	<< "                       items themself." << std::endl
	<< std::endl
	<< "Where [args] depends on the specified action:" << std::endl
	<< " default action        1 or more herds." << std::endl
	<< " -p, --package         1 or more herds." << std::endl
	<< " -d, --dev             1 or more developers." << std::endl
	<< "Both the default action and the --dev action support an 'all' target" << std::endl
	<< "that show all of the devs or herds.  If both --dev and --package are" << std::endl
	<< "specified, " << PACKAGE << " will display all packages maintained by" << std::endl
	<< "the specified developer." << std::endl
#else

	<< " -h              Display this help message." << std::endl
	<< " -V              Display version information." << std::endl
	<< std::endl
	<< "Where [options] can be any of the following:" << std::endl
	<< " -p              Look up packages by herd." << std::endl
	<< " -d              Look up herds by developer." << std::endl
	<< " -H <file>       Specify location of herds.xml." << std::endl
	<< " -o <file>       Send output to the specified file" << std::endl
	<< "                 instead of stdout." << std::endl
	<< " -f              Force a fetch of herds.xml." << std::endl
	<< " -v              Display verbose output." << std::endl
	<< " -q              Don't display labels and fancy colors. Use this"
	<< std::endl
	<< "                 option to pipe herdstat output to other programs"
	<< std::endl
	<< " -D              Display debugging messages." << std::endl
	<< " -t              Display elapsed time of XML parsing." << std::endl
	<< " -c              Display the number of items instead of the" << std::endl
	<< "                 items themself." << std::endl
	<< std::endl
	<< "Where [args] depends on the specified action:" << std::endl
	<< " default action  1 or more herds." << std::endl
	<< " -p              1 or more herds." << std::endl
	<< " -d              1 or more developers." << std::endl
	<< "Both the default action and the -d action support an 'all' target" << std::endl
	<< "that show all of the devs or herds.  If both -d and -p are specified," << std::endl
	<< PACKAGE << " will display all packages maintained by the specified" << std::endl
	<< "developer." << std::endl

#endif /* HAVE_GETOPT_LONG */

	<< std::endl
	<< PACKAGE << " respects the HERDS environment variable." << std::endl
	<< "Set it in your shell rcfile to permanently set the location of "
	<< "your herds.xml" << std::endl;
}

int
handle_opts(int argc, char **argv, std::vector<std::string> *args)
{
    int key, opt_index = 0;

    while (true)
    {
#ifdef HAVE_GETOPT_LONG
	key = getopt_long(argc, argv, short_opts, long_opts, &opt_index);
#else
	key = getopt(argc, argv, short_opts);
#endif /* HAVE_GETOPT_LON)G */

	if (key == -1)
	    break;

	switch (key)
	{
	    /* --dev */
	    case 'd':
		if (optget("action", options_action_T) != action_unspecified and
		    optget("action", options_action_T) != action_pkg)
		    throw args_one_action_only_E();
		if (optget("action", options_action_T) == action_pkg)
		    optset("dev", bool, true);
		else
		    optset("action", options_action_T, action_dev);
		break;
	    /* --package */
	    case 'p':
		if (optget("action", options_action_T) != action_unspecified and
		    optget("action", options_action_T) != action_dev)
		    throw args_one_action_only_E();
		if (optget("action", options_action_T) == action_dev)
		    optset("dev", bool, true);
		optset("action", options_action_T, action_pkg);
		break;
	    /* --outfile */
	    case 'o':
		if (strcmp(optarg, "stdout") != 0)
		{
		    if (strcmp(optarg, "stderr") == 0)
			optset("outstream", std::ostream *, &std::cerr);
		    optset("outfile", std::string, optarg);
		    optset("quiet", bool, true);
		    optset("timer", bool, false);
		}
		break;
	    /* --fetch */
	    case 'f':
		optset("fetch", bool, true);
		break;
	    /* --verbose */
	    case 'v':
		optset("verbose", bool, true);
		break;
	    /* --quiet */
	    case 'q':
		optset("quiet", bool, true);
		break;
	    /* --count */
	    case 'c':
		optset("count", bool, true);
		optset("quiet", bool, true);
		break;
	    /* --herdsxml */
	    case 'H':
		optset("herds.xml", std::string, optarg);
		break;
	    /* --debug */
	    case 'D':
		optset("timer", bool, true);
		optset("debug", bool, true);
		break;
	    /* --timer */
	    case 't':
		if (optget("outfile", std::string) == "stdout")
		    optset("timer", bool, true);
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
    else if (optget("action", options_action_T) != action_unspecified)
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

    /* try to determine current columns, otherwise use default */
    optset("maxcol", size_t, util::getcols());

    /* HERDS */
    char *result = getenv("HERDS");
    if (result)
	optset("herds.xml", std::string, result);
    else
    {
	/* if a fetched copy exists and is newer than 24hrs, use it.
	 * see the explanation below where we fetch as to why
	 * we go through the trouble of doing this. */
	struct stat s;
	if ((stat(fetched_location.c_str(), &s) == 0) and
	    ((time(NULL) - s.st_mtime) < 86400) and (s.st_size > 0))
	    optset("herds.xml", std::string, fetched_location);
    }

    try
    { 
	std::vector<std::string> nonopt_args;

	/* handle command line options */
	if (handle_opts(argc, argv, &nonopt_args) != 0)
	    throw args_E();

	if (nonopt_args.empty())
	{
	    optset("action", options_action_T, action_stats);
	    optset("quiet", bool, false);
	}

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

	    optset("all", bool, true);
	}

	if (optget("debug", bool))
	    options.dump(*optget("outstream", std::ostream *));

	if (not optget("quiet", bool) and optget("fetch", bool) and nonopt_args.empty())
	    optset("verbose", bool, true);

	/* every action handler needs to parse herds.xml for one reason
	 * or another, so let's get it over with. */
	struct stat s;
	try
	{
	    if (optget("fetch", bool) and
		(optget("herds.xml", std::string).find("://") == std::string::npos))
		optset("herds.xml", std::string, default_herdsxml);

	    if (optget("herds.xml", std::string).find("://") != std::string::npos)
	    {
		/* NOTE: ideally we'd love to only fetch it when the timestamp
		 * and size are different, however, because the default
		 * location to download herds.xml is from ViewCVS, there is no
		 * Last-Modified header, meaning wget can't do timestamps :( */

		if (not optget("quiet", bool))
		{
		    std::cout
			<< "Fetching herds.xml..."
			<< std::endl << std::endl;
		}

		/* backup cached copy if it exists so we can use it
		 * if fetching fails */
		if (util::is_file(fetched_location))
		    util::copy_file(fetched_location, fetched_location + ".bak");

		/* fetch it */
		if (util::fetch(optget("herds.xml", std::string), fetched_location,
		    optget("verbose", bool)) != 0)
		    throw fetch_E();

		/* because we tell wget to clobber the file, if fetching fails for
		 * some reason, it'll truncate the old one - make sure the file
		 * is greater than 0 bytes */
		if ((stat(fetched_location.c_str(), &s) == 0) and (s.st_size > 0))
		    optset("herds.xml", std::string, fetched_location);
		else
		    throw fetch_E();

		/* remove back up copy */
		unlink((fetched_location + ".bak").c_str());
	    }
	}
	catch (const fetch_E &e)
	{
	    std::cerr << "Error fetching " << optget("herds.xml", std::string)
		<< std::endl << std::endl;

	    /* if we can't fetch it but have an old cached copy, use it */
	    if (util::is_file(fetched_location + ".bak"))
	    {
		std::cerr << "Using cached copy... ";
		util::move_file(fetched_location + ".bak", fetched_location);
		optset("herds.xml", std::string, fetched_location);
	    }
		
	    std::cerr
		<< "If you have a local copy, specify it using -H or by "
		<< std::endl << "setting the HERDS environment variable."
		<< std::endl;

	    if (stat(fetched_location.c_str(), &s) != 0)
		return EXIT_FAILURE;
	    else if (s.st_size == 0)
	    {
		unlink(fetched_location.c_str());
		return EXIT_FAILURE;
	    }
	    else
		std::cerr << std::endl;
	}

	/* make sure herds.xml exists */
	if (not util::is_file(optget("herds.xml", std::string)))
	    throw bad_fileobject_E("%s: %s", optget("herds.xml", std::string).c_str(),
		strerror(errno));

	/* if there's no nonoption args, we're done */
	if (optget("fetch", bool) and nonopt_args.empty())
	    return EXIT_SUCCESS;

	std::auto_ptr<HerdsXMLHandler_T> handler(new HerdsXMLHandler_T());
	try
	{
	    if (optget("timer", bool))
		timer.start();

	    /* parse herds.xml */
	    XMLParser_T parser(&(*handler));
	    parser.parse(optget("herds.xml", std::string));

	    if (optget("timer", bool))
		timer.stop();
	}
	catch (const XMLParser_E &e)
	{
	    std::cerr << "Error parsing '" << e.file() << "': "	<< e.error()
		<< std::endl;
	    return EXIT_FAILURE;
	}

	/* setup outfile */
	std::ostream *outstream = NULL;
	if (optget("outfile", std::string) != "stdout" and
	    optget("outfile", std::string) != "stderr")
	{
	    outstream = new std::ofstream(optget("outfile", std::string).c_str());
	    if (not *outstream)
		throw bad_fileobject_E("%s: %s", optget("outfile", std::string).c_str(),
		    strerror(errno));
	    optset("outstream", std::ostream *, outstream);
	}

	/* set common format attributes */
	util::color_map_T color;
	formatter_T output(outstream);
	output.set_colors(true);
	output.set_quiet(optget("quiet", bool));
	output.set_labelcolor(color[green]);

	/* set default action */
	if (optget("action", options_action_T) == action_unspecified)
	    optset("action", options_action_T, action_herd);

	std::map<options_action_T, action_handler_T * > handlers;
	handlers[action_herd]  = new action_herd_handler_T();
	handlers[action_dev]   = new action_dev_handler_T();
	handlers[action_pkg]   = new action_pkg_handler_T();
	handlers[action_stats] = new action_stats_handler_T();

	action_handler_T *action_handler =
	    handlers[optget("action", options_action_T)];
	if (action_handler)
	{
	    try
	    {
		if ((*action_handler)(handler->herds, nonopt_args) != EXIT_SUCCESS)
		    return EXIT_FAILURE;
	    }
	    catch (action_E)
	    {
		return EXIT_FAILURE;
	    }
	}
	else
	    throw args_unimplemented_E();

	if (outstream)
	    delete outstream;

	if (optget("timer", bool))
	    throw timer_E();
    }
    catch (const bad_fileobject_E &e)
    {
	std::cerr << e.what() << std::endl;
	return EXIT_FAILURE;
    }
    catch (const timer_E)
    {
	std::cout
	    << "Took " << timer.elapsed() << "ms to parse herds.xml." << std::endl;
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
