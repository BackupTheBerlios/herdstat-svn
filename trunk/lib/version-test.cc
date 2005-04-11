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

    std::vector<std::string> possibles = portage::find_package("/usr/portage", argv[1]);
    if (possibles.size() > 1)
    {
        std::cerr << argv[1] << " is ambiguous. Possible matches are:" << std::endl;
        std::vector<std::string>::iterator i;
        for (i = possibles.begin() ; i != possibles.end() ; ++i)
            std::cerr << *i << std::endl;
        return EXIT_FAILURE;
    }
    else if (possibles.empty())
    {
        std::cerr << argv[1] << " doesn't seem to exist." << std::endl;
        return EXIT_FAILURE;
    }

    portage::versions_T versions;
    util::dir_T pkgdir(std::string("/usr/portage/") + possibles.front());
    util::dir_T::iterator d;
    for (d = pkgdir.begin() ; d != pkgdir.end() ; ++d)
    {
        std::string::size_type pos = d->rfind(".ebuild");
        if (pos == std::string::npos)
            continue;

        versions.insert(new portage::version_string_T(*d));
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

//    std::vector<portage::version_suffix_T> svec;
//    svec.push_back(portage::version_suffix_T("1.0_alpha"));
//    svec.push_back(portage::version_suffix_T("1.0_beta3"));
//    svec.push_back(portage::version_suffix_T("1.0_p3"));
//    svec.push_back(portage::version_suffix_T("1.0"));
//    svec.push_back(portage::version_suffix_T("1.0_p7"));
//    svec.push_back(portage::version_suffix_T("1.0_p"));
//    svec.push_back(portage::version_suffix_T("1.0.1"));
//    svec.push_back(portage::version_suffix_T("1.0_pre20050301"));
//    svec.push_back(portage::version_suffix_T("1.0_rc7"));

//    std::stable_sort(svec.begin(), svec.end(), sortSuffix);

//    std::cout << std::endl;
//    std::vector<portage::version_suffix_T>::iterator i;
//    for (i = svec.begin() ; i != svec.end() ; ++i)
//        std::cout << i->suffix() << i->version() << std::endl;

    for (v = versions.begin() ; v != versions.end() ; ++v)
        delete *v;
}

/* vim: set tw=80 sw=4 et : */
