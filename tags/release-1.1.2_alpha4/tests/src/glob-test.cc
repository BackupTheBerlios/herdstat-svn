/*
 * herdstat -- tests/src/glob-test.cc
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
#include <algorithm>
#include <functional>
#include <cstdlib>
#include <herdstat/exceptions.hh>
#include <herdstat/util/glob.hh>

int main()
{
    try
    {
        /* test glob_T */
        const util::glob_T results("portdir/*/*/*.ebuild");
        util::glob_T::const_iterator i;
        for (i = results.begin() ; i != results.end() ; ++i)
            std::cout << *i << std::endl;

        /* test patternMatch functor */
        util::glob_T::size_type n = std::count_if(results.begin(), results.end(),
                std::bind1st(util::patternMatch(), "*foo-1.10*"));
        if (n == 0)
            throw Exception("*foo-1.10* not found in results vector.");

        std::cout << std::endl
            << "The pattern '*foo-1.10*' appears " << n
            << " times in the output above." << std::endl;
    }
    catch (const BaseException &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/* vim: set tw=80 sw=4 et : */
