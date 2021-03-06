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
#include <vector>
#include <memory>
#include <algorithm>

#ifdef HAVE_GETOPT_H
# include <getopt.h>
#endif

#include <herdstat/exceptions.hh>
#include <herdstat/libherdstat_version.hh>
#include <herdstat/util/string.hh>
#include <herdstat/util/getcols.hh>
#include <herdstat/util/functional.hh>
#include <herdstat/portage/exceptions.hh>

// {{{ local includes
#include "common.hh"
#include "handler_map.hh"
#include "xmlinit.hh"
#include "formatter.hh"
#include "io/handler.hh"
#include "io/stream.hh"
#include "io/readline.hh"
#include "io/batch.hh"
#include "io/gui.hh"
#include "action/handler.hh"
#include "action/away.hh"
#include "action/dev.hh"
#include "action/find.hh"
#include "action/herd.hh"
#include "action/keywords.hh"
#include "action/meta.hh"
#include "action/pkg.hh"
#include "action/stats.hh"
#include "action/versions.hh"
#include "action/which.hh"
// }}}

#define HERDSTATRC_GLOBAL   SYSCONFDIR"/herdstatrc"
#define HERDSTATRC_LOCAL    /*HOME*/"/.herdstatrc"

using namespace herdstat;
using namespace herdstat::portage;
using namespace herdstat::xml;

// {{{ getopt stuff
static const char *short_opts = "H:o:hVvDdtpqFcnmwNErfaA:L:C:U:Tki:S";

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
//    {"field",	    required_argument,	0,  'X'},
    {"keywords",    no_argument,	0,  'k'},
    {"iomethod",    required_argument,  0,  'i'},
    {"no-spinner",  no_argument,        0,  'S'},
    { 0, 0, 0, 0 }
};
#endif /* HAVE_GETOPT_LONG */
// }}}

// {{{ version()
static void
version()
{
    std::cout << PACKAGE << "-" << VERSION
        << " w/libherdstat-" << LIBHERDSTAT_VERSION_MAJOR << "."
        << LIBHERDSTAT_VERSION_MINOR << "." << LIBHERDSTAT_VERSION_MICRO
	<< " (built: " << __DATE__ << " " << __TIME__ << ")" << std::endl;
    std::cout << "Options:"
#ifdef DEBUG
    << " +debug"
#else
    << " -debug"
#endif
#ifdef READLINE_FRONTEND
    << " +readline"
#else
    << " -readline"
#endif
#ifdef QT_FRONTEND
    << " +qt"
#else
    << " -qt"
#endif
#ifdef GTK_FRONTEND
    << " +gtk"
#else
    << " -gtk"
#endif

    << std::endl;
}
// }}}

static void
usage()
{
    std::cerr
	<< "usage: " << PACKAGE << " [options] [args]" << std::endl
	<< "Use --help to see more detailed usage information." << std::endl;
}

// {{{ help()
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
	<< " -k, --keywords          Display keywords for the specified packages." << std::endl
	<< " -i, --iomethod          Front-end to use (readline, batch)." << std::endl
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
        << " -S, --no-spinner        Don't show a spinner when performing a query." << std::endl
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
// }}}

//static void
//parse_fields(const std::vector<std::string>& fields, Options *options)
//{
//    /* --field */
//    std::vector<std::string>::const_iterator i;
//    for (i = fields.begin() ; i != fields.end() ; ++i)
//    {
//        std::vector<std::string> parts;
//        util::split(*i, std::back_inserter(parts), ",");
//        if (parts.size() != 2 or (parts[0].empty() or parts[1].empty()))
//            throw argsInvalidField();

//        options->add_field(std::make_pair(parts[0], parts[1]));
//    }
//}

static bool
handle_opts(int argc, char **argv, Query *q)
{
    Options& options(GlobalOptions());
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

	// {{{
	switch (key)
	{
	    case 'T':
		break;
	    /* --dev */
	    case 'd':
		if (q->action() != "unspecified" and
		    q->action() != "pkg" and
		    q->action() != "meta")
		    throw argsOneActionOnly();
		if (q->action() == "pkg" or
		    q->action() == "meta")
		    options.set_dev(true);
		else
		    q->set_action("dev");
		break;
	    /* --package */
	    case 'p':
		if (q->action() != "unspecified" and
		    q->action() != "dev" and
		    q->action() != "meta")
		    throw argsOneActionOnly();
		if (q->action() == "dev")
		    options.set_dev(true);
		if (q->action() == "meta")
		    options.set_meta(true);
		q->set_action("pkg");
		break;
	    /* --metadata */
	    case 'm':
		if (q->action() != "unspecified" and
		    q->action() != "pkg" and
		    q->action() != "dev" and
		    q->action() != "find")
		    throw argsOneActionOnly();

		if (q->action() == "pkg" or
		    q->action() == "dev" or
		    q->action() == "find")
		    options.set_meta(true);
		else
		    q->set_action("meta");
		break;
	    /* --which */
	    case 'w':
		if (q->action() != "unspecified")
		    throw argsOneActionOnly();
		q->set_action("which");
		break;
	    /* --find */
	    case 'f':
		if (q->action() != "unspecified" and
		    q->action() != "meta")
		    throw argsOneActionOnly();
		if (q->action() == "meta")
		    options.set_meta(true);
		q->set_action("find");
		break;
	    /* --versions */
	    case '\b':
		if (q->action() != "unspecified")
		    throw argsOneActionOnly();
		q->set_action("versions");
		break;
            /* --keywords */
	    case 'k':
		if (q->action() != "unspecified")
		    throw argsOneActionOnly();
		q->set_action("keywords");
		break;
	    /* --away */
	    case 'a':
		if (q->action() != "unspecified")
		    throw argsOneActionOnly();
		q->set_action("away");
		break;
	    /* --fetch */
	    case 'F':
		if (q->action() != "unspecified")
		    throw argsOneActionOnly();
		q->set_action("fetch");
		options.set_fetch(true);
                GlobalXMLInit();
		break;
	    /* --field */
//            case 'X':
//                fields.push_back(optarg);
//                break;
	    /* --no-overlay */
	    case 'N':
		options.set_overlay(false);
		break;
	    /* --outfile */
	    case 'o':
		if (strcmp(optarg, "stdout") != 0)
		{
		    if (strcmp(optarg, "stderr") == 0)
			options.set_outstream(&std::cerr);
		    options.set_outfile(optarg);
		    options.set_quiet(true);
		    options.set_timer(false);
		}
		break;
	    /* --regex */
	    case 'r':
		options.set_regex(true);
		break;
	    /* --extended */
	    case 'E':
		options.set_regex(true);
		options.set_eregex(true);
		break;
	    /* --no-herd */
	    case '\n':
		options.set_with_herd("no-herd");
		break;
	    /* --with-herd */
	    case '\v':
		options.set_with_herd(optarg);
		break;
	    /* --no-maintainer */
	    case '\r':
		options.set_with_dev("none");
		break;
	    /* --with-maintainer */
	    case '\t':
		options.set_with_dev(optarg);
		break;
	    /* --nometacache */
	    case '\f':
		options.set_metacache(false);
		break;
	    /* --verbose */
	    case 'v':
		options.set_verbose(true);
		break;
	    /* --quiet */
	    case 'q':
		options.set_quiet(true);
		options.set_color(false);
		break;
	    /* --count */
	    case 'c':
		options.set_count(true);
		options.set_quiet(true);
		break;
	    /* --nocolor */
	    case 'n':
		options.set_color(false);
		break;
	    /* --gentoo-cvs */
	    case 'C':
		options.set_cvsdir(optarg);
		break;
	    /* --userinfo */
	    case 'U':
		options.set_userinfoxml(optarg);
		break;
	    /* --herdsxml */
	    case 'H':
		options.set_herdsxml(optarg);
		break;
	    /* --devaway */
	    case 'A':
		options.set_devawayxml(optarg);
		break;
	    /* --localstatedir */
	    case 'L':
		options.set_localstatedir(optarg);
		break;
	    /* --debug */
	    case 'D':
		options.set_timer(true);
		options.set_debug(true);
		break;
	    /* --timer */
	    case 't':
		if (options.outfile() == "stdout")
		    options.set_timer(true);
		break;
	    /* --qa */
	    case '\a':
		options.set_qa(true);
		break;
	    /* --iomethod */
	    case 'i':
		options.set_iomethod(optarg);
		break;
	    /* --version */
	    case 'V':
		throw argsVersion();
		break;
            /* --no-spinner */
            case 'S':
                options.set_spinner(false);
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
    // }}}

    /* --field */
//    parse_fields(fields, &options);

    if (optind < argc)
        std::transform(argv+optind, argv+argc,
            std::back_inserter(*q), util::stringify<char *>);
    else
    {
	/* actions that are allowed to have 0 non-option args */
	const std::string& action = q->action();
//        if (action == "dev" and fields.empty())
//            throw argsUsage();

	if (action != "unspecified" and
	    action != "meta" and
	    action != "dev" and
	    action != "versions" and
	    action != "fetch" and
	    action != "keywords" and
	    (options.iomethod() == "stream"))
	    throw argsUsage();
    }

    return true;
}

int
main(int argc, char **argv)
{
    BacktraceContext c("main()");

    Options& options(GlobalOptions());
    std::ostream *outstream = NULL;

    /* set iomethod based on argv[0] */
    if (std::strcmp(argv[0], PACKAGE) != 0)
    {
        if (util::basename(argv[0]) == PACKAGE"-rl")
            options.set_iomethod("readline");
        else if (util::basename(argv[0]) == PACKAGE"-gtk")
            options.set_iomethod("gtk");
        else if (util::basename(argv[0]) == PACKAGE"-qt")
            options.set_iomethod("qt");
    }

    bool test = false;
    if (argc > 1)
    {
        if (std::strcmp(argv[1], "-") == 0)
            options.set_iomethod("batch");
        else if ((std::strcmp(argv[1], "--TEST") == 0) or
                 (std::strcmp(argv[1], "-T") == 0))
            test = true;
    }

    /* save column width; need to use a constant value if
     * we're in "test mode" so results don't vary based on
     * terminal width */
    options.set_maxcol(test ? 79 : (util::getcols() - 1));

    try
    { 
	Query q;
	
	/* handle rc file(s) */
	if (not test)
            options.read_configs();

	char *getenv_result = NULL;
	if ((getenv_result = std::getenv("HERDSTAT_FRONTEND")))
	    options.set_iomethod(getenv_result);

	/* handle command line options */
	if (not handle_opts(argc, argv, &q))
	    throw argsException();

        if (not q.empty() and q.front().second == "all")
        {
            q.erase(q.begin());
            q.set_all(true);
        }

	/* set path to herds.xml and userinfo.xml if --gentoo-cvs was specified */
	if (not options.cvsdir().empty())
	{
	    const std::string gentoocvs(options.cvsdir());
	    if (not util::is_dir(gentoocvs))
		throw FileException(gentoocvs);

	    /* only set if it wasnt specified on the command line */
	    if (options.herdsxml().empty())
		options.set_herdsxml(gentoocvs+"/gentoo/misc/herds.xml");
	    if (options.userinfoxml().empty())
		options.set_userinfoxml(gentoocvs+"/gentoo/xml/htdocs/proj/en/devrel/roll-call/userinfo.xml");
	}

	/* setup output stream */
	if (options.outfile() != "stdout" and options.outfile() != "stderr")
	{
	    outstream = new std::ofstream(options.outfile().c_str());
	    if (not *outstream)
		throw FileException(options.outfile());
	    options.set_outstream(outstream);
	}
        /* setup locale only if outstream is stdout||stderr */
        else
        {
            try
            {
                std::locale locale("");
                options.set_locale(locale.name());
                options.outstream().imbue(locale);
            }
            catch (const std::runtime_error&)
            {
                throw BadLocale();
            }
        }

	/* set default action */
	if (q.action() == "unspecified")
	    q.set_action(q.all() ? "herd" : q.empty() ? "stats" : "herd");

	/* setup action handlers */
	HandlerMap<ActionHandler>& handlers(GlobalHandlerMap<ActionHandler>());
	handlers.insert(std::make_pair("away", new AwayActionHandler()));
	handlers.insert(std::make_pair("dev",  new DevActionHandler()));
	handlers.insert(std::make_pair("find", new FindActionHandler()));
	handlers.insert(std::make_pair("herd", new HerdActionHandler()));
	handlers.insert(std::make_pair("keywords", new KeywordsActionHandler()));
	handlers.insert(std::make_pair("meta", new MetaActionHandler()));
	handlers.insert(std::make_pair("pkg", new PkgActionHandler()));
	handlers.insert(std::make_pair("stats", new StatsActionHandler()));
	handlers.insert(std::make_pair("versions", new VersionsActionHandler()));
	handlers.insert(std::make_pair("which", new WhichActionHandler()));

	/* setup I/O handlers */
	HandlerMap<IOHandler>& iohandlers(GlobalHandlerMap<IOHandler>());
	iohandlers.insert(std::make_pair("stream", new StreamIOHandler()));
	iohandlers.insert(std::make_pair("batch", new BatchIOHandler()));

#ifdef READLINE_FRONTEND
	iohandlers.insert(std::make_pair("readline", new ReadLineIOHandler()));
#endif
#ifdef QT_FRONTEND
	iohandlers.insert(std::make_pair("qt", new GuiIOHandler(argc, argv)));
#endif
#ifdef GTK_FRONTEND
	iohandlers.insert(std::make_pair("gtk", new GuiIOHandler(argc, argv)));
#endif

        /* main loop (except if action == fetch) */
	while (q.action() != "fetch")
	{
	    std::auto_ptr<Query> query(new Query());
	    const std::string& iomethod(options.iomethod());

	    IOHandler *handler = iohandlers[iomethod];
	    if (not handler)
		throw IOHandlerUnimplemented(iomethod);

	    /* we've already filled a query object when parsing the
	     * command line options so use it instead. */
	    if (iomethod == "stream")
		std::swap(*query, q);

	    /* execute I/O handler */
	    if (not (*handler)(query.get()))
		break;
	}

	if (outstream)
	    delete outstream;

        /* clean up handler maps */
        std::for_each(handlers.begin(), handlers.end(),
            util::compose_f_gx(
                util::DeleteAndNullify<ActionHandler>(),
                util::Second<HandlerMap<ActionHandler>::value_type>()));

        std::for_each(iohandlers.begin(), iohandlers.end(),
            util::compose_f_gx(
                util::DeleteAndNullify<IOHandler>(),
                util::Second<HandlerMap<IOHandler>::value_type>()));
    }
    // {{{ catches
    catch (const ActionException&)
    {
        return EXIT_FAILURE;
    }
    catch (const ActionUnimplemented& e)
    {
	std::cerr << "Invalid action '" << e.what() << "'.  Try --help."
	    << std::endl;
	return EXIT_FAILURE;
    }
    catch (const QAException& e)
    {
	std::cerr << e.backtrace(":\n  * ") << "QA Violation: " << e.what() << std::endl;
	return EXIT_FAILURE;
    }
    catch (const ParserException& e)
    {
	std::cerr << "Oops!" << std::endl << "  * "
            << e.backtrace(":\n  * ") << "Error parsing "
            << e.file() << ": " << e.error()
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
	std::cerr << e.backtrace(":\n  * ") << "Error parsing date '" << e.what() << "'." << std::endl;
	return EXIT_FAILURE;
    }
    catch (const FetchException)
    {
	return EXIT_FAILURE;
    }
    catch (const BadRegex& e)
    {
	std::cerr << e.backtrace(":\n  * ") << "Bad regular expression." << std::endl;
	return EXIT_FAILURE;
    }
    catch (const argsInvalidField&)
    {
        std::cerr << "Format for --field is \"--field=field,criteria\"." << std::endl
            << "For example: --field=status,active" << std::endl;
        return EXIT_FAILURE;
    }
    catch (const argsHelp&)
    {
	help();
	return EXIT_SUCCESS;
    }
    catch (const argsVersion&)
    {
	version();
	return EXIT_SUCCESS;
    }
    catch (const argsUsage&)
    {
	usage();
	return EXIT_FAILURE;
    }
    catch (const argsException&)
    {
	usage();
	return EXIT_FAILURE;
    }
    catch (const Exception& e)
    {
	std::cerr << "Oops!" << std::endl  << "  * " << e.backtrace(":\n  * ")
            << e.what() << std::endl;
	return EXIT_FAILURE;
    }
    catch (const BaseException& e)
    {
	std::cerr << "Unhandled exception: " << e.backtrace(":\n  * ")
            << e.what() << std::endl;
	return EXIT_FAILURE;
    }
    catch (...)
    {
        std::cerr << "Caught unknown exception!" << std::endl;
        return EXIT_FAILURE;
    }
    // }}}

    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 fdm=marker et : */
