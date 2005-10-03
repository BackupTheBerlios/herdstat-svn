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

#include <herdstat/exceptions.hh>
#include <herdstat/util/string.hh>
#include <herdstat/xml/init.hh>
#include <herdstat/portage/exceptions.hh>

#include "common.hh"
#include "rc.hh"
#include "formatter.hh"
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
#define HERDSTATRC_LOCAL    /*HOME*/"/.herdstatrc"

static const char *short_opts = "H:o:hVvDdtpqFcnmwNErfaA:L:C:U:TX:";

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
    /* specify the location of devaway.xml. */
    {"devaway",	    required_argument,	0,  'A'},
    /* specify the location of userinfo.xml */
    {"userinfo",    required_argument,  0,  'U'},
    /* specify Gentoo CVS directory */
    {"gentoo-cvs",  required_argument,  0,  'C'},
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
    {"TEST",	    no_argument,	0,  'T'},
    {"field",	    required_argument,	0,  'X'},
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
	<< "     --field <field,criteria>" << std::endl
	<< "                         Search by field (for use with --dev).  Possible fields" << std::endl
	<< "                         are user,name,birthday,joined,status,location." << std::endl
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
	<< " -C, --gentoo-cvs <dir>  Specify location of Gentoo CVS directory.  herds.xml and" << std::endl
	<< "                         userinfo.xml will be looked up relative to this directory." << std::endl
	<< " -H, --herdsxml <file>   Specify location of herds.xml." << std::endl
	<< " -A, --devaway  <file>   Specify location of devaway.xml." << std::endl
	<< " -U, --userinfo <file>   Specify location of userinfo.xml." << std::endl
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

static void
parse_fields(const std::vector<std::string>& fields)
{
    /* --field */
    std::vector<std::string>::const_iterator i;
    for (i = fields.begin() ; i != fields.end() ; ++i)
    {
	std::vector<std::string> parts(util::split(*i, ','));
	if (parts.size() != 2 or (parts[0].empty() or parts[1].empty()))
	    throw argsInvalidField();

	options::add_field(fields_type::value_type(parts[0], parts[1]));
    }
}

static int
handle_opts(int argc, char **argv, opts_type *args)
{
    std::vector<std::string> fields;
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
	    case 'T':
		break;
	    /* --dev */
	    case 'd':
		if (options::action() != action_unspecified and
		    options::action() != action_pkg and
		    options::action() != action_meta)
		    throw argsOneActionOnly();
		if (options::action() == action_pkg or
		    options::action() == action_meta)
		    options::set_dev(true);
		else
		    options::set_action(action_dev);
		break;
	    /* --package */
	    case 'p':
		if (options::action() != action_unspecified and
		    options::action() != action_dev and
		    options::action() != action_meta)
		    throw argsOneActionOnly();
		if (options::action() == action_dev)
		    options::set_dev(true);
		if (options::action() == action_meta)
		    options::set_meta(true);
		options::set_action(action_pkg);
		break;
	    /* --metadata */
	    case 'm':
		if (options::action() != action_unspecified and
		    options::action() != action_pkg and
		    options::action() != action_dev and
		    options::action() != action_find)
		    throw argsOneActionOnly();

		if (options::action() == action_pkg or
		    options::action() == action_dev or
		    options::action() == action_find)
		    options::set_meta(true);
		else
		    options::set_action(action_meta);
		break;
	    /* --which */
	    case 'w':
		if (options::action() != action_unspecified)
		    throw argsOneActionOnly();
		options::set_action(action_which);
		break;
	    /* --find */
	    case 'f':
		if (options::action() != action_unspecified and
		    options::action() != action_meta)
		    throw argsOneActionOnly();
		if (options::action() == action_meta)
		    options::set_meta(true);
		options::set_action(action_find);
		break;
	    /* --versions */
	    case '\b':
		if (options::action() != action_unspecified)
		    throw argsOneActionOnly();
		options::set_action(action_versions);
		break;
	    /* --away */
	    case 'a':
		if (options::action() != action_unspecified)
		    throw argsOneActionOnly();
		options::set_action(action_away);
		break;
	    /* --fetch */
	    case 'F':
		if (options::action() != action_unspecified)
		    throw argsOneActionOnly();
		options::set_action(action_fetch);
		break;
	    /* --field */
	    case 'X':
		fields.push_back(optarg);
		break;
	    /* --no-overlay */
	    case 'N':
		options::set_overlay(false);
		break;
	    /* --outfile */
	    case 'o':
		if (strcmp(optarg, "stdout") != 0)
		{
		    if (strcmp(optarg, "stderr") == 0)
			options::set_outstream(&std::cerr);
		    options::set_outfile(optarg);
		    options::set_quiet(true);
		    options::set_timer(false);
		}
		break;
	    /* --regex */
	    case 'r':
		options::set_regex(true);
		break;
	    /* --extended */
	    case 'E':
		options::set_regex(true);
		options::set_eregex(true);
		break;
	    /* --no-herd */
	    case '\n':
		options::set_with_herd("no-herd");
		break;
	    /* --with-herd */
	    case '\v':
		options::set_with_herd(optarg);
		break;
	    /* --no-maintainer */
	    case '\r':
		options::set_with_dev("none");
		break;
	    /* --with-maintainer */
	    case '\t':
		options::set_with_dev(optarg);
		break;
	    /* --nometacache */
	    case '\f':
		options::set_metacache(false);
		break;
	    /* --verbose */
	    case 'v':
		options::set_verbose(true);
		break;
	    /* --quiet */
	    case 'q':
		options::set_quiet(true);
		options::set_color(false);
		break;
	    /* --count */
	    case 'c':
		options::set_count(true);
		options::set_quiet(true);
		break;
	    /* --nocolor */
	    case 'n':
		options::set_color(false);
		break;
	    /* --gentoo-cvs */
	    case 'C':
		options::set_cvsdir(optarg);
		break;
	    /* --userinfo */
	    case 'U':
		options::set_userinfoxml(optarg);
		break;
	    /* --herdsxml */
	    case 'H':
		options::set_herdsxml(optarg);
		break;
	    /* --devaway */
	    case 'A':
		options::set_devawayxml(optarg);
		break;
	    /* --localstatedir */
	    case 'L':
		options::set_localstatedir(optarg);
		break;
	    /* --debug */
	    case 'D':
		options::set_timer(true);
		options::set_debug(true);
		break;
	    /* --timer */
	    case 't':
		if (options::outfile() == "stdout")
		    options::set_timer(true);
		break;
	    /* --qa */
	    case '\a':
		options::set_qa(true);
		break;
	    /* --version */
	    case 'V':
		throw argsVersion();
		break;
	    /* --help */
	    case 'h':
		throw argsHelp();
		break;
	    case 0:
		throw argsUsage();
		break;
	    default:
		throw argsException();
		break;
	}
    }

    /* --field */
    parse_fields(fields);

    if (optind < argc)
    {
	while (optind < argc)
	    args->push_back(argv[optind++]);
    }
    else
    {
	/* actions that are allowed to have 0 non-option args */
	options_action_T action = options::action();
	if (action == action_dev and fields.empty())
	    throw argsUsage();

	if (action != action_unspecified and
	    action != action_meta and
	    action != action_dev and
	    action != action_versions and
	    action != action_fetch)
	    throw argsUsage();
    }

    return 0;
}

int
main(int argc, char **argv)
{
    options opts;
    std::map<options_action_T, action_handler_T * > handlers;
    std::ostream *outstream = NULL;

    /* we need to know if -T or --TEST was specified before 
     * we parse the command line options. */
    const bool test = ((argc > 1) and
		      ((std::strcmp(argv[1], "--TEST") == 0) or
		       (std::strcmp(argv[1], "-T") == 0)));

    /* save column width */
    options::set_maxcol((test ? 80 : util::getcols()));

    try
    { 
	opts_type nonopt_args;
	
	/* handle rc file(s) */
	if (not test) { rc_T rc; }

	/* handle command line options */
	if (handle_opts(argc, argv, &nonopt_args) != 0)
	    throw argsException();

	/* set path to herds.xml and userinfo.xml if --gentoo-cvs was specified */
	if (not options::cvsdir().empty())
	{
	    const std::string gentoocvs(options::cvsdir());
	    if (not util::is_dir(gentoocvs))
		throw FileException(gentoocvs);

	    /* only set if it wasnt specified on the command line */
	    if (options::herdsxml().empty())
		options::set_herdsxml(gentoocvs+"/gentoo/misc/herds.xml");
	    if (options::userinfoxml().empty())
		options::set_userinfoxml(gentoocvs+"/gentoo/xml/htdocs/proj/en/devrel/roll-call/userinfo.xml");
	}

	/* initialize XML stuff */
	xml::Init init(options::qa());

	if (not options::fields().empty() and not nonopt_args.empty())
	{
	    std::cerr << "--field doesn't make much sense when specified" << std::endl
		      << "with an additional non-optional argument." << std::endl;
	    return EXIT_FAILURE;
	}

	if (options::regex() and nonopt_args.size() > 1)
	{
	    std::cerr << "You may only specify one regular expression."
		<< std::endl;
	    return EXIT_FAILURE;
	}

	/* remove duplicates; also has the nice side advantage
	 * of sorting the output */
	std::sort(nonopt_args.begin(), nonopt_args.end());
	nonopt_args.erase(std::unique(nonopt_args.begin(),
		nonopt_args.end()), nonopt_args.end());

	/* did the user specify the all target? */
	if (not options::regex() and
	    (std::find(nonopt_args.begin(),
		       nonopt_args.end(), "all") != nonopt_args.end()))
	{
	    options::set_all(true);
	    nonopt_args.clear();
	    nonopt_args.push_back("all");
	}

	/* setup output stream */
	if (options::outfile() != "stdout" and options::outfile() != "stderr")
	{
	    outstream = new std::ofstream(options::outfile().c_str());
	    if (not *outstream)
		throw FileException(options::outfile());
	    options::set_outstream(outstream);
	}
	else
	{
	    /* save locale name */
	    try
	    {
		options::set_locale(std::locale("").name());
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
//        if (options::debug())
//            options.dump(*options::outstream());	

	/* set locale */
	options::outstream()->imbue(std::locale(options::locale().c_str()));

	/* set common format attributes */
	util::color_map_T color;
	formatter_T output;
	output.set_colors(options::color());
	output.set_quiet(options::quiet());
	output.set_labelcolor(color[options::labelcolor()]);
	output.set_highlightcolor(color[options::hlcolor()]);
	output.set_devaway_color(color[red]);

	/* add highlights */
	output.add_highlight(util::current_user());
	output.add_highlight(util::get_user_from_email(util::current_user()));
	
	/* user-defined highlights */
	{
	    const std::vector<std::string> hv(util::split(options::highlights()));
	    output.add_highlights(hv);
	}

	/* set default action */
	if (options::action() == action_unspecified and nonopt_args.empty())
	    options::set_action(action_stats);
	else if (options::action() == action_unspecified and
		not nonopt_args.empty())
	    options::set_action(action_herd);

	/* setup action handlers */
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

	action_handler_T *action_handler = handlers[options::action()];

	if (action_handler)
	{
	    try
	    {
		if ((*action_handler)(nonopt_args) != EXIT_SUCCESS)
		    return EXIT_FAILURE;
	    }
	    catch (const ActionException)
	    {
		return EXIT_FAILURE;
	    }
	}
	else
	    throw argsUnimplemented();

	if (outstream)
	    delete outstream;

	std::map<options_action_T, action_handler_T * >::iterator m;
	for (m = handlers.begin() ; m != handlers.end() ; ++m)
	    if (m->second) delete m->second;
    }
    catch (const portage::QAException& e)
    {
	std::cerr << "QA Violation: " << e.what() << std::endl;
	return EXIT_FAILURE;
    }
    catch (const xml::ParserException& e)
    {
	std::cerr << "Error parsing " << e.file() << ": " << e.error()
	    << std::endl;
	return EXIT_FAILURE;
    }
    catch (const BadOption& e)
    {
	std::cerr << e.what() << std::endl;
	return EXIT_FAILURE;
    }
    catch (const BadDate& e)
    {
	std::cerr << "Error parsing date '" << e.what() << "'." << std::endl;
	return EXIT_FAILURE;
    }
    catch (const FormatException &e)
    {
	std::cerr << e.what() << std::endl;
	return EXIT_FAILURE;
    }
    catch (const FetchException)
    { return EXIT_FAILURE; }
    catch (const BadRegex)
    {
	std::cerr << "Bad regular expression." << std::endl;
	return EXIT_FAILURE;
    }
    catch (const FileException &e)
    {
	std::cerr << e.what() << std::endl;
	return EXIT_FAILURE;
    }
    catch (const ErrnoException &e)
    {
	std::cerr << e.what() << std::endl;
	return EXIT_FAILURE;
    }
    catch (const argsInvalidField)
    {
        std::cerr << "Format for --field is \"--field=field,criteria\"." << std::endl
            << "For example: --field=status,active" << std::endl;
        return EXIT_FAILURE;
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
    catch (const BaseException& e)
    {
	std::cerr << "Unhandled exception: " << e.what() << std::endl;
	return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
