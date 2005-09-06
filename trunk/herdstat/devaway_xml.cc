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
#include <herdstat/portage/devaway_xml.hh>

using namespace portage;

int
main(int argc, char **argv)
{
    try
    {
        if (argc != 2)
            throw Exception("usage: devaway_xml <path>");

        std::string path(argv[1]);
        devaway_xml devaway(path);

        const Herd& devs = devaway.devs();

        std::cout << "Away developers(" << devs.size() << ")" << std::endl;

        for (Herd::const_iterator i = devs.begin() ; i != devs.end() ; ++i)
            std::cout << i->user() << ": " << i->awaymsg() << std::endl;
    }
    catch (const BaseException &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
