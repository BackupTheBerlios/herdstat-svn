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

#ifdef HAVE_GETOPT_H
# include <getopt.h>
#endif

#include "common.hh"
#include "rc.hh"
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
#include "action_away_handler.hh"
#include "action_fetch_handler.hh"

#define HERDSTATRC_GLOBAL   SYSCONFDIR"/herdstatrc"
#define HERDSTATRC_LOCAL    "/.herdstatrc"

static const char *short_opts = "H:o:hVvDdtpqFcnmwNErfaA:L:";

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
    {"no-maintainer", no_argument,	0,  '\r'},
    {"with-maintainer", required_argument,0,'\t'},
    /* force a fetch of herds.xml */
    {"fetch",	    no_argument,	0,  'F'},
    /* time how long it takes for XML parsing */
    {"timer",	    no_argument,	0,  't'},
    /* instead of displaying devs for a herd, display herds for a dev */
    {"dev",	    no_argument,	0,  'd'},
    /* specify the location of a local herds.xml */
    {"herdsxml",    required_argument,	0,  'H'},
    /* specify the location of devaway.html. */
    {"devaway",	    required_argument,	0,  'A'},
    /* specify local statedir */
    {"localstatedir", required_argument,0,  'L'},
    /* show package stats for the specified herds */
    {"package",	    no_argument,	0,  'p'},
    /* display package metadata information */
    {"metadata",    no_argument,	0,  'm'},
    {"which",	    no_argument,	0,  'w'},
    {"versions",    no_argument,	0,  '\b'},
    {"find",	    no_argument,	0,  'f'},
    {"away",	    no_argument,	0,  'a'},
    /* specify a file to write the output to */
    {"outfile",	    required_argument,	0,  'o'},
    {"no-overlay",  no_argument,	0,  'N'},
    {"regex",	    no_argument,	0,  'r'},
    {"extended",    no_argument,	0,  'E'},
    {"qa",	    no_argument,	0,  '\a'},
    {"nometacache",  no_argument,	0,  '\f'},
    { 0, 0, 0, 0 }
};
#endif /* HAVE_GETOPT_LONG */

static void
version()
{
    std::cout << PACKAGE << "-" << VERSION << std::endl;
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
	<< "usage: " << PACKAGE << " [options] [args]" << std::endl

#ifdef HAVE_GETOPT_LONG
	<< " -h, --help              Display this help message." << std::endl
	<< " -V, --version           Display version information." << std::endl
	<< std::endl
	<< "Where [options] can be any of the following:" << std::endl
	<< " -p, --package           Look up packages by herd." << std::endl
	<< " -d, --dev               Look up herds by developer." << std::endl
	<< " -m, --metadata          Look up metadata by package/category." << std::endl
	<< " -w, --which             Look up full path to ebuild for specified packages." << std::endl
	<< " -f, --find              Look up category/package for the specified packages." << std::endl
	<< " -a, --away              Look up away information for the specified developers." << std::endl
	<< "     --versions          Look up versions of specified packages." << std::endl
	<< "     --with-herd <regex> When used in conjunction with --package and --dev," << std::endl
	<< "                         display all packages that belong to a herd that matches" << std::endl
	<< "                         the specified regular expression." << std::endl
	<< "     --no-herd           Shorthand for --with-herd=no-herd" << std::endl
	<< "     --with-maintainer <regex>" << std::endl
	<< "                         When used in conjunction with --package, display" << std::endl
	<< "                         all packages that are maintained by a developer matching" << std::endl
	<< "                         the specified regular expression." << std::endl
	<< "     --no-maintainer     Shorthand for --with-maintainer=none" << std::endl
	<< " -N, --no-overlay        Don't search overlay(s) in PORTDIR_OVERLAY." << std::endl
	<< "     --nometacache       When used in conjunction with --package, don't use any" << std::endl
	<< "                         cached query results." << std::endl
	<< " -r, --regex             Display results matching the specified regular" << std::endl
	<< "                         expression." << std::endl
	<< " -E, --extended          Use extended regular expressions. Implies --regex." << std::endl
	<< " -H, --herdsxml <file>   Specify location of herds.xml." << std::endl
	<< " -A, --devaway  <file>   Specify location of devaway.html." << std::endl
	<< " -o, --outfile  <file>   Send output to the specified file" << std::endl
	<< "                         instead of stdout." << std::endl
	<< " -L, --localstatedir <dir>" << std::endl
	<< "                         Specify local state directory." << std::endl
	<< " -F, --fetch             Force a fetch of herds.xml." << std::endl
	<< " -v, --verbose           Display verbose output." << std::endl
	<< " -q, --quiet             Don't display labels and fancy colors. Use this" << std::endl
	<< "                         option to pipe herdstat output to other programs" << std::endl
	<< " -D, --debug             Display debugging messages." << std::endl
	<< " -t, --timer             Display elapsed time of XML parsing." << std::endl
	<< " -c, --count             Display the number of items instead of the" << std::endl
	<< "                         items themself." << std::endl
	<< " -n, --nocolor           Don't display colored output." << std::endl
	<< "     --qa                Complain loudly if a QA-related problem occurs." << std::endl
	<< std::endl
	<< "Where [args] depends on the specified action:" << std::endl
	<< " default action          1 or more herds." << std::endl
	<< " -p, --package           1 or more herds." << std::endl
	<< " -d, --dev               1 or more developers." << std::endl
	<< " -m, --metadata          1 or more categories/packages." << std::endl
	<< " -w, --which             1 or more packages." << std::endl
	<< " -f, --find              1 or more packages." << std::endl
	<< " -a, --away              1 or more developers." << std::endl
	<< "     --versions          1 or more packages." << std::endl
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
	<< " -a              Look up away information for the specified developers." << std::endl
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
	<< " -f              1 or more packages." << std::endl
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

static int
handle_opts(int argc, char **argv, opts_type *args)
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
		    optget("action", options_action_T) != action_pkg and
		    optget("action", options_action_T) != action_meta)
		    throw args_one_action_only_E();
		if (optget("action", options_action_T) == action_pkg or
		    optget("action", options_action_T) == action_meta)
		    optset("dev", bool, true);
		else
		    optset("action", options_action_T, action_dev);
		break;
	    /* --package */
	    case 'p':
		if (optget("action", options_action_T) != action_unspecified and
		    optget("action", options_action_T) != action_dev and
		    optget("action", options_action_T) != action_meta)
		    throw args_one_action_only_E();
		if (optget("action", options_action_T) == action_dev)
		    optset("dev", bool, true);
		if (optget("action", options_action_T) == action_meta)
		    optset("meta", bool, true);
		optset("action", options_action_T, action_pkg);
		break;
	    /* --metadata */
	    case 'm':
		if (optget("action", options_action_T) != action_unspecified and
		    optget("action", options_action_T) != action_pkg and
		    optget("action", options_action_T) != action_dev and
		    optget("action", options_action_T) != action_find)
		    throw args_one_action_only_E();

		if (optget("action", options_action_T) == action_pkg or
		    optget("action", options_action_T) == action_dev or
		    optget("action", options_action_T) == action_find)
		    optset("meta", bool, true);
		else
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
		if (optget("action", options_action_T) != action_unspecified and
		    optget("action", options_action_T) != action_meta)
		    throw args_one_action_only_E();
		if (optget("action", options_action_T) == action_meta)
		    optset("meta", bool, true);
		optset("action", options_action_T, action_find);
		break;
	    /* --versions */
	    case '\b':
		if (optget("action", options_action_T) != action_unspecified)
		    throw args_one_action_only_E();
		optset("action", options_action_T, action_versions);
		break;
	    /* --away */
	    case 'a':
		if (optget("action", options_action_T) != action_unspecified)
		    throw args_one_action_only_E();
		optset("action", options_action_T, action_away);
		break;
	    /* --fetch */
	    case 'F':
		if (optget("action", options_action_T) != action_unspecified)
		    throw args_one_action_only_E();
		optset("action", options_action_T, action_fetch);
		break;
	    /* --no-overlay */
	    case 'N':
		optset("overlay", bool, false);
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
		optset("with-herd", std::string, "no-herd");
		break;
	    /* --with-herd */
	    case '\v':
		optset("with-herd", std::string, optarg);
		break;
	    /* --no-maintainer */
	    case '\r':
		optset("with-maintainer", std::string, "none");
		break;
	    /* --with-maintainer */
	    case '\t':
		optset("with-maintainer", std::string, optarg);
		break;
	    /* --noquerycache */
	    case '\f':
		optset("metacache", bool, false);
		break;
	    /* --verbose */
	    case 'v':
		optset("verbose", bool, true);
		break;
	    /* --quiet */
	    case 'q':
		optset("quiet", bool, true);
		optset("color", bool, false);
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
		optset("herds.xml", std::string, optarg);
		break;
	    /* --devaway */
	    case 'A':
		optset("devaway.location", std::string, optarg);
		break;
	    /* --localstatedir */
	    case 'L':
		optset("localstatedir", std::string, optarg);
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
	    /* --qa */
	    case '\a':
		optset("qa", bool, true);
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
    {
	/* actions that are allowed to have 0 non-option args */
	options_action_T action = optget("action", options_action_T);
	if (action != action_unspecified and
	    action != action_meta and
	    action != action_versions and
	    action != action_fetch)
	    throw args_usage_E();
    }

    return 0;
}

int
main(int argc, char **argv)
{
    options_T options;

    /* try to determine current columns, otherwise use default */
    optset("maxcol", std::size_t, util::getcols());

    try
    { 
	opts_type nonopt_args;
	
	/* handle rc file(s) */
	{ rc_T rc; }

	/* handle command line options */
	if (handle_opts(argc, argv, &nonopt_args) != 0)
	    throw args_E();

	/* remove duplicates; also has the nice side advantage
	 * of sorting the output */
	std::sort(nonopt_args.begin(), nonopt_args.end());
	opts_type::iterator pos =
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

	/* setup output stream */
	std::ostream *outstream = NULL;
	if (optget("outfile", std::string) != "stdout" and
	    optget("outfile", std::string) != "stderr")
	{
	    outstream = new std::ofstream(optget("outfile", std::string).c_str());
	    if (not *outstream)
		throw bad_fileobject_E(optget("outfile", std::string));
	    optset("outstream", std::ostream *, outstream);
	}
	else
	{
	    /* save locale name */
	    try
	    {
		optset("locale", std::string, std::locale("").name());
	    }
	    catch (const std::runtime_error)
	    {
		std::string error("Invalid locale");
		char *result = std::getenv("LC_ALL");
		if (result)
		    error += " '" + std::string(result) + "'.";
		std::cerr << error << std::endl;
		return EXIT_FAILURE;
	    }
	}

	/* dump options */
	if (optget("debug", bool))
	    options.dump(*optget("outstream", std::ostream *));	

	/* set locale */
	optget("outstream", std::ostream *)->imbue
	    (std::locale(optget("locale", std::string).c_str()));

	/* set common format attributes */
	util::color_map_T color;
	formatter_T output;
	output.set_colors(optget("color", bool));
	output.set_quiet(optget("quiet", bool));
	output.set_labelcolor(color[optget("label.color", std::string)]);
	output.set_highlightcolor(color[optget("highlight.color", std::string)]);
	output.set_devaway_color(color[red]);

	/* add highlights */
	output.add_highlight(util::current_user());
	output.add_highlight(util::get_user_from_email(util::current_user()));
	
	/* user-defined highlights */
//        {
//            const std::vector<std::string> hv(optget("highlights",
//                std::string).split());
//            std::vector<std::string>::const_iterator i;
//            for (i = hv.begin() ; i != hv.end() ; ++i)
//                output.add_highlight(*i);
//        }

	{
	    const std::vector<std::string> hv(util::split(optget("highlights",
		std::string)));
	    output.add_highlights(hv);
	}

	/* set default action */
	if (optget("action", options_action_T) == action_unspecified
	    and nonopt_args.empty())
	    optset("action", options_action_T, action_stats);
	else if (optget("action", options_action_T) == action_unspecified
	    and not nonopt_args.empty())
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
	handlers[action_away]     = new action_away_handler_T();
	handlers[action_fetch]    = new action_fetch_handler_T();

	action_handler_T *action_handler =
	    handlers[optget("action", options_action_T)];

	if (action_handler)
	{
	    try
	    {
		if ((*action_handler)(nonopt_args) != EXIT_SUCCESS)
		    return EXIT_FAILURE;
	    }
	    catch (const action_E)
	    {
		return EXIT_FAILURE;
	    }
	}
	else
	    throw args_unimplemented_E();

	if (outstream)
	    delete outstream;

    }
    catch (const XMLParser_E &e)
    {
	std::cerr << "Error parsing '" << e.file() << "': " << e.error()
	    << std::endl;
	return EXIT_FAILURE;
    }
    catch (const XMLWriter_E &e)
    {
	std::cerr << "Error writing '" << e.file() << "': " << e.error()
	    << std::endl;
	return EXIT_FAILURE;
    }
    catch (const format_E &e)
    {
	std::cerr << e.what() << std::endl;
	return EXIT_FAILURE;
    }
    catch (const fetch_E)
    { return EXIT_FAILURE; }
    catch (const portage::qa_E)
    { return EXIT_FAILURE; }
    catch (const util::bad_regex_E)
    {
	std::cerr << "Bad regular expression." << std::endl;
	return EXIT_FAILURE;
    }
    catch (const util::bad_fileobject_E &e)
    {
	std::cerr << e.what() << std::endl;
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
    catch (const rc_E)
    {
	return EXIT_FAILURE;
    }
    catch (const herdstat_base_E &e)
    {
	std::cerr << "Unhandled exception: " << e.what() << std::endl;
	return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
