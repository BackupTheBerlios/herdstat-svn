/*
 * herdstat -- lib/version-test.cc
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

#include <utility>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <cstdlib>
#include <cassert>

#include "util.hh"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "usage: " << util::basename(argv[0]) << " <category/package>"
            << std::endl;
        return 1;
    }

    const std::string portdir = portage::portdir();

    std::string package;
    try
    {
        package = portage::find_package(portdir, argv[1]);
    }
    catch (const portage::ambiguous_pkg_E &e)
    {
        std::cerr << e.name()
            << " is ambiguous. Possible matches are:"
            << std::endl << std::endl;

        std::vector<std::string>::const_iterator i;
        for (i = e.packages.begin() ; i != e.packages.end() ; ++i)
            std::cerr << *i << std::endl;
        return EXIT_FAILURE;
    }
    catch (const portage::nonexistent_pkg_E &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    portage::versions_T versions;
    util::dir_T pkgdir(portdir + "/" + package);
    util::dir_T::iterator d;
    for (d = pkgdir.begin() ; d != pkgdir.end() ; ++d)
    {
        std::string::size_type pos = d->rfind(".ebuild");
        if (pos == std::string::npos)
            continue;

        assert(versions.insert(*d));
    }

    portage::versions_T::iterator v;
    for (v = versions.begin() ; v != versions.end() ; ++v)
    {
        portage::version_string_T *version = *v;
//        std::cout << "Full version: " << (*version)() << std::endl;
//        std::cout << "P: " << (*version)["P"] << std::endl;
//        std::cout << "PN: " << (*version)["PN"] << std::endl;
//        std::cout << "PV: " << (*version)["PV"] << std::endl;
//        std::cout << "PVR: " << (*version)["PVR"] << std::endl;
//        std::cout << "PR: " << (*version)["PR"] << std::endl;
//        std::cout << "PF: " << (*version)["PF"] << std::endl;
//        std::cout << std::endl;
        std::cout << (*version)() << std::endl;
    }
}

/* vim: set tw=80 sw=4 et : */
