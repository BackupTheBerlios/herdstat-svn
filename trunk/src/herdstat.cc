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

#include <locale>
#include <map>
#include <vector>
#include <algorithm>
#include <glibmm/error.h>

#ifdef HAVE_GETOPT_H
# include <getopt.h>
#endif

#include "common.hh"
#include "herds_xml.hh"
#include "formatter.hh"
#include "xmlparser.hh"
#include "action_herd_handler.hh"
#include "action_pkg_handler.hh"
#include "action_dev_handler.hh"
#include "action_find_handler.hh"
#include "action_meta_handler.hh"
#include "action_stats_handler.hh"
#include "action_which_handler.hh"
#include "action_versions_handler.hh"

static const char *short_opts = "H:o:hVvDdtpqFcnmwNErf";

#ifdef HAVE_GETOPT_LONG
static struct option long_opts[] =
{
    {"version",	    no_argument,	0,  'V'},
    {"help",	    no_argument,	0,  'h'},
    {"verbose",	    no_argument,	0,  'v'},
    {"quiet",	    no_argument,	0,  'q'},
    {"debug",	    no_argument,	0,  'D'},
    {"count",	    no_argument,	0,  'c'},
    {"nocolor",	    no_argument,	0,  'n'},
    {"no-herd",	    no_argument,	0,  '\n'},
    {"with-herd",   required_argument,	0,  '\v'},
    /* force a fetch of herds.xml */
    {"fetch",	    no_argument,	0,  'F'},
    /* time how long it takes for XML parsing */
    {"timer",	    no_argument,	0,  't'},
    /* instead of displaying devs for a herd, display herds for a dev */
    {"dev",	    no_argument,	0,  'd'},
    /* specify the location of a local herds.xml */
    {"herdsxml",    required_argument,	0,  'H'},
    /* show package stats for the specified herds */
    {"package",	    no_argument,	0,  'p'},
    /* display package metadata information */
    {"metadata",    no_argument,	0,  'm'},
    {"which",	    no_argument,	0,  'w'},
    {"versions",    no_argument,	0,  '\b'},
    {"find",	    no_argument,	0,  'f'},
    /* specify a file to write the output to */
    {"outfile",	    required_argument,	0,  'o'},
    {"no-overlay",  no_argument,	0,  'N'},
    {"regex",	    no_argument,	0,  'r'},
    {"extended",    no_argument,	0,  'E'},
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
    version();
    std::cout << std::endl
	<< "usage: " << PACKAGE << " [options] [args]" << std::endl

#ifdef HAVE_GETOPT_LONG
	<< " -h, --help             Display this help message." << std::endl
	<< " -V, --version          Display version information." << std::endl
	<< std::endl
	<< "Where [options] can be any of the following:" << std::endl
	<< " -p, --package          Look up packages by herd." << std::endl
	<< " -d, --dev              Look up herds by developer." << std::endl
	<< " -m, --metadata         Look up metadata by package/category." << std::endl
	<< " -w, --which            Look up full path to ebuild for specified packages." << std::endl
	<< " -f, --find             Look up category/package for the specified packages." << std::endl
	<< "     --versions	    Look up versions of specified packages." << std::endl
	<< "     --with-herd <herd> When used in conjunction with --package and --dev," << std::endl
	<< "                        display all packages that belong to the specified herd." << std::endl
	<< "     --no-herd          Shorthand for --with-herd=no-herd" << std::endl
	<< " -N, --no-overlay       Don't search overlay(s) in PORTDIR_OVERLAY." << std::endl
	<< " -r, --regex	    Display results matching the specified regular expression." << std::endl
	<< " -E, --extended         Use extended regular expressions. Implies --regex." << std::endl
	<< " -H, --herdsxml <file>  Specify location of herds.xml." << std::endl
	<< " -o, --outfile  <file>  Send output to the specified file" << std::endl
	<< "                        instead of stdout." << std::endl
	<< " -F, --fetch            Force a fetch of herds.xml." << std::endl
	<< " -v, --verbose          Display verbose output." << std::endl
	<< " -q, --quiet            Don't display labels and fancy colors. Use this"
	<< std::endl
	<< "                        option to pipe herdstat output to other programs"
	<< std::endl
	<< " -D, --debug            Display debugging messages." << std::endl
	<< " -t, --timer            Display elapsed time of XML parsing." << std::endl
	<< " -c, --count            Display the number of items instead of the" << std::endl
	<< "                        items themself." << std::endl
	<< " -n, --nocolor          Don't display colored output." << std::endl
	<< std::endl
	<< "Where [args] depends on the specified action:" << std::endl
	<< " default action         1 or more herds." << std::endl
	<< " -p, --package          1 or more herds." << std::endl
	<< " -d, --dev              1 or more developers." << std::endl
	<< " -m, --metadata         1 or more categories/packages." << std::endl
	<< " -w, --which            1 or more packages." << std::endl
	<< std::endl
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
	<< " -m              Look up metadata by package/category." << std::endl
	<< " -f              Look up category/package of the specified packages." << std::endl
	<< " -w              Look up full path to ebuild for specified packages." << std::endl
	<< " -N              Don't search overlay(s) in PORTDIR_OVERLAY." << std::endl
	<< " -r              Display results matching the specified regular expression." << std::endl
	<< " -E              Use extended regular expressions.  Implies -r." << std::endl
	<< " -H <file>       Specify location of herds.xml." << std::endl
	<< " -o <file>       Send output to the specified file" << std::endl
	<< "                 instead of stdout." << std::endl
	<< " -F              Force a fetch of herds.xml." << std::endl
	<< " -v              Display verbose output." << std::endl
	<< " -q              Don't display labels and fancy colors. Use this"
	<< std::endl
	<< "                 option to pipe herdstat output to other programs"
	<< std::endl
	<< " -D              Display debugging messages." << std::endl
	<< " -t              Display elapsed time of XML parsing." << std::endl
	<< " -c              Display the number of items instead of the" << std::endl
	<< "                 items themself." << std::endl
	<< " -n              Don't display colored output." << std::endl
	<< std::endl
	<< "Where [args] depends on the specified action:" << std::endl
	<< " default action  1 or more herds." << std::endl
	<< " -p              1 or more herds." << std::endl
	<< " -d              1 or more developers." << std::endl
	<< " -m              1 or more categories/packages." << std::endl
	<< " -w              1 or more packages." << std::endl
	<< std::endl
	<< "Both the default action and the -d action support an 'all' target" << std::endl
	<< "that show all of the devs or herds.  If both -d and -p are specified," << std::endl
	<< PACKAGE << " will display all packages maintained by the specified" << std::endl
	<< "developer." << std::endl

#endif /* HAVE_GETOPT_LONG */

	<< std::endl
	<< PACKAGE << " respects the HERDS environment variable." << std::endl
	<< "Set it in your shell rcfile to permanently set the location of "
	<< "your herds.xml." << std::endl;
}

int
handle_opts(int argc, char **argv, std::vector<util::string> *args)
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
	    /* --metadata */
	    case 'm':
		if (optget("action", options_action_T) != action_unspecified)
		    throw args_one_action_only_E();
		optset("action", options_action_T, action_meta);
		break;
	    /* --which */
	    case 'w':
		if (optget("action", options_action_T) != action_unspecified)
		    throw args_one_action_only_E();
		optset("action", options_action_T, action_which);
		break;
	    /* --find */
	    case 'f':
		if (optget("action", options_action_T) != action_unspecified)
		    throw args_one_action_only_E();
		optset("action", options_action_T, action_find);
		break;
	    /* --versions */
	    case '\b':
		if (optget("action", options_action_T) != action_unspecified)
		    throw args_one_action_only_E();
		optset("action", options_action_T, action_versions);
		break;
	    case 'N':
		optset("overlay", bool, false);
		break;
	    /* --outfile */
	    case 'o':
		if (strcmp(optarg, "stdout") != 0)
		{
		    if (strcmp(optarg, "stderr") == 0)
			optset("outstream", std::ostream *, &std::cerr);
		    optset("outfile", util::string, optarg);
		    optset("quiet", bool, true);
		    optset("timer", bool, false);
		}
		break;
	    /* --regex */
	    case 'r':
		optset("regex", bool, true);
		break;
	    /* --extended */
	    case 'E':
		optset("regex", bool, true);
		optset("eregex", bool, true);
		break;
	    /* --no-herd */
	    case '\n':
		optset("with-herd", util::string, "no-herd");
		break;
	    /* --with-herd */
	    case '\v':
		optset("with-herd", util::string, optarg);
		break;
	    /* --fetch */
	    case 'F':
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
	    /* --nocolor */
	    case 'n':
		optset("color", bool, false);
		break;
	    /* --herdsxml */
	    case 'H':
		optset("herds.xml", util::string, optarg);
		break;
	    /* --debug */
	    case 'D':
		optset("timer", bool, true);
		optset("debug", bool, true);
		break;
	    /* --timer */
	    case 't':
		if (optget("outfile", util::string) == "stdout")
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
    else if ((optget("action", options_action_T) != action_unspecified) and
	    (optget("action", options_action_T) != action_meta) and
	    (optget("action", options_action_T) != action_versions))
	throw args_usage_E();

    return 0;
}

int
main(int argc, char **argv)
{
    options_T options;
    util::color_map_T color;

    /* try to determine current columns, otherwise use default */
    optset("maxcol", std::size_t, util::getcols());

    try
    { 
	std::vector<util::string> nonopt_args;

	/* handle command line options */
	if (handle_opts(argc, argv, &nonopt_args) != 0)
	    throw args_E();

	if (nonopt_args.empty() and
	    optget("action", options_action_T) != action_meta and
	    optget("action", options_action_T) != action_versions and
	    not optget("fetch", bool))
	{
	    optset("action", options_action_T, action_stats);
	    optset("quiet", bool, false);
	}

	/* remove duplicates; also has the nice side advantage
	 * of sorting the output */
	std::sort(nonopt_args.begin(), nonopt_args.end());
	std::vector<util::string>::iterator pos =
	    std::unique(nonopt_args.begin(), nonopt_args.end());
	if (pos != nonopt_args.end())
	    nonopt_args.erase(pos);

	/* did the user specify the all target? */
	if (std::find(nonopt_args.begin(),
		nonopt_args.end(), "all") != nonopt_args.end())
	{
	    optset("all", bool, true);
	    nonopt_args.clear();
	    nonopt_args.push_back("all");
	}

	/* --fetch and no options: fetch herds.xml and exit */
	if (optget("fetch", bool) and nonopt_args.empty())
	{
	    if (not optget("quiet", bool))
		optset("verbose", bool, true);

	    herds_xml_T herdsxml(true);
	    return EXIT_SUCCESS;
	}

	/* setup output stream */
	std::ostream *outstream = NULL;
	if (optget("outfile", util::string) != "stdout" and
	    optget("outfile", util::string) != "stderr")
	{
	    outstream = new std::ofstream(optget("outfile", util::string).c_str());
	    if (not *outstream)
		throw bad_fileobject_E(optget("outfile", util::string));
	    optset("outstream", std::ostream *, outstream);
	}
	else
	{
	    /* save locale name */
	    try
	    {
		optset("locale", util::string, std::locale("").name());
	    }
	    catch (const std::runtime_error)
	    {
		util::string error("Invalid locale");
		char *result = std::getenv("LC_ALL");
		if (result)
		    error += " '" + util::string(result) + "'.";
		std::cerr << error << std::endl;
		return EXIT_FAILURE;
	    }
	}

	/* dump options */
	if (optget("debug", bool))
	    options.dump(*optget("outstream", std::ostream *));	

	/* set locale */
	optget("outstream", std::ostream *)->imbue
	    (std::locale(optget("locale", util::string).c_str()));

	/* set common format attributes */
	formatter_T output;
	output.set_colors(optget("color", bool));
	output.set_quiet(optget("quiet", bool));
	output.set_labelcolor(color[green]);
	output.set_highlightcolor(color[yellow]);
	output.add_highlight(util::current_user());
	output.add_highlight(util::get_user_from_email(util::current_user()));

	/* set default action */
	if (optget("action", options_action_T) == action_unspecified)
	    optset("action", options_action_T, action_herd);

	/* setup action handlers */
	std::map<options_action_T, action_handler_T * > handlers;
	handlers[action_herd]     = new action_herd_handler_T();
	handlers[action_dev]      = new action_dev_handler_T();
	handlers[action_pkg]      = new action_pkg_handler_T();
	handlers[action_meta]     = new action_meta_handler_T();
	handlers[action_stats]    = new action_stats_handler_T();
	handlers[action_which]    = new action_which_handler_T();
	handlers[action_versions] = new action_versions_handler_T();
	handlers[action_find]     = new action_find_handler_T();

	action_handler_T *action_handler =
	    handlers[optget("action", options_action_T)];

	if (action_handler)
	{
	    try
	    {
		if ((*action_handler)(nonopt_args) != EXIT_SUCCESS)
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

    }
    catch (const Glib::ConvertError &e)
    {
	std::cerr << e.what() << std::endl;
	std::cerr
	    << "There is currently a bug in glibmm that causes an exception"
	    << std::endl
	    << "to be thrown when trying to display a UTF-8 character.  Until"
	    << std::endl
	    << "it is fixed, this message will be displayed.  Sorry for the inconvenience."
	    << std::endl;
	return EXIT_FAILURE;
    }
    catch (const Glib::Error &e)
    {
	std::cerr << e.what() << std::endl;
	return EXIT_FAILURE;
    }
    catch (const XMLParser_E &e)
    {
	std::cerr << "Error parsing '" << e.file() << "': " << e.error()
	    << std::endl;
	return EXIT_FAILURE;
    }
    catch (const fetch_E)
    {
	return EXIT_FAILURE;
    }
    catch (const util::base_E &e)
    {
	std::cerr << e.what() << std::endl;
	return EXIT_FAILURE;
    }
    catch (const errno_error_E &e)
    {
	std::cerr << e.what() << std::endl;
	return EXIT_FAILURE;
    }
    catch (const timer_E &e)
    {
	std::cout
	    << "Took " << e.what() << "ms to parse herds.xml." << std::endl;
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
