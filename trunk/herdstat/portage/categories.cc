/*
 * herdstat -- herdstat/portage/categories.cc
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

#include <fstream>
#include <iterator>
#include <herdstat/portage/exceptions.hh>
#include <herdstat/portage/config.hh>
#include <herdstat/portage/categories.hh>

#define CATEGORIES      "/profiles/categories"
#define CATEGORIES_USER "/etc/portage/categories"

namespace portage {
/*** static members *********************************************************/
bool Categories::_init = false;
Categories::container_type Categories::_s;
/****************************************************************************/
Categories::Categories(bool validate)
    : _portdir(config_T::portdir()), _validate(validate)
{
    this->fill();
}
/****************************************************************************/
Categories::Categories(const std::string& portdir, bool validate)
    : _portdir(portdir), _validate(validate)
{
    this->fill();
}
/****************************************************************************/
void
Categories::fill()
{
    if (_init)
        return;

    std::string line;
    /* read main categories file */
    {
        std::ifstream stream((_portdir+CATEGORIES).c_str());
        if (not stream)
            throw FileException(_portdir+CATEGORIES);

        std::size_t n = 0;
        while (std::getline(stream, line))
        {
            /* virtual isn't a real category */
            if (line == "virtual")
                continue;

            /* bail if validate mode is enabled and
             * the category does not exist. */
            if (_validate and not util::is_dir(_portdir+"/"+line))
            {
                std::cerr << "QA Violation: " << _portdir << CATEGORIES
                    << ":" << ++n << std::endl << "category '" << line
                    << "' is listed but does not exist." << std::endl;
                throw QAException();
            }

            _s.insert(line);
        }
    }

    /* read user categories file */
    if (util::is_file(CATEGORIES_USER))
    {
        std::ifstream stream(CATEGORIES_USER);
        if (not stream)
            throw FileException(CATEGORIES_USER);

        _s.insert(std::istream_iterator<std::string>(stream),
                  std::istream_iterator<std::string>());
    }

    _init = true;
}
/****************************************************************************/
} // namespace portage

/* vim: set tw=80 sw=4 et : */
