/*
 * herdstat -- herdstat/devaway_xml.cc
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
#include <cstdlib>
#include <herdstat/exceptions.hh>
#include <herdstat/util/string.hh>
#include <herdstat/util/file.hh>
#include <herdstat/portage/devaway_xml.hh>

using namespace portage;
using namespace util;

int
main(int argc, char **argv)
{
    try
    {
        if (argc != 2)
            throw Exception("usage: devaway_xml <path>");

        std::string path(argv[1]);
        assert(util::is_file(path));
        devaway_xml devaway;
        devaway.parse(path);

        const Developers& devs = devaway.devs();

        std::cout << "Away developers(" << devs.size() << ")" << std::endl;

        Developers::const_iterator i;
        for (i = devs.begin() ; i != devs.end() ; ++i)
            std::cout << i->user() << ": " << tidy_whitespace(i->awaymsg())
                << std::endl;

        std::cout << std::endl;

        assert((i = devs.find("lv")) != devs.end());
        std::cout << i->user() << " - " << tidy_whitespace(i->awaymsg())
            << std::endl;
    }
    catch (const BaseException &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
