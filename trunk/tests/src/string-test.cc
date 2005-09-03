/*
 * herdstat -- tests/src/string-test.cc
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
#include <vector>
#include <cstdlib>
#include <cassert>
#include <herdstat/util/string.hh>

int main(int argc, char **argv)
{
    std::string s("Mary had a little lamb!");
    std::cout << s << std::endl;
    
    /* test util::split() */
    std::vector<std::string> v(util::split(s));
    assert(v.size() == 5);
    std::vector<std::string>::iterator i;
    std::cout << "[ ";
    for (i = v.begin() ; i != v.end() ; ++i)
    {
        std::cout << "'" << *i << "'";
        if ((i+1) != v.end())
            std::cout << ", ";
    }
    std::cout << " ]" << std::endl;

    /* util::lowercase */
    std::cout << util::lowercase(s) << std::endl;

    /* util::destringify */
    std::cout << util::destringify<int>("10") << std::endl
        << util::destringify<long>("01") << std::endl
        << util::destringify<unsigned long>("385233535") << std::endl
        << util::destringify<float>("4.29") << std::endl
        << util::destringify<bool>("false") << std::endl;

    std::cout << util::unhtmlify("&lt;jedi mind trick>") << std::endl;

    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
