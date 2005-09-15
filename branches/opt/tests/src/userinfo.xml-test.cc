/*
 * herdstat -- herdstat/userinfo_xml.cc
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
#include <unistd.h>
#include <herdstat/exceptions.hh>
#include <herdstat/portage/userinfo_xml.hh>

using namespace portage;

int
main(int argc, char **argv)
{
    try
    {
        if (argc != 3)
            throw Exception("usage: userinfo_xml <path> <dev>");

        std::string path(argv[1]);
        std::string dev(argv[2]);

        userinfo_xml userinfo(path);
        const Developers& devs = userinfo.devs();

        std::cout << "Size: " << devs.size() << std::endl;

        Developers::const_iterator i = devs.find(dev);
        if (i == devs.end())
            throw Exception(dev + " doesn't seem to exist.");

//        for (Herd::const_iterator i = devs.begin() ; i != devs.end() ; ++i)
//        {
//        if(*i == dev)
//        {
        std::cout << "Name:       " << (*i)->name() << "(" << (*i)->user() << ")" << std::endl;
        std::cout << "Email:      " << (*i)->email() << std::endl;
        std::cout << "PGP Key ID: " << (*i)->pgpkey() << std::endl;
        std::cout << "Joined on:  " << (*i)->joined() << std::endl;
        std::cout << "Birthday:   " << (*i)->birthday() << std::endl;
        std::cout << "Status:     " << (*i)->status() << std::endl;
        std::cout << "Roles:      " << (*i)->role() << std::endl;
        std::cout << "Location:   " << (*i)->location() << std::endl;
//        }
//        }
    }
    catch (const BaseException &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
