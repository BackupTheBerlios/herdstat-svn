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
#include <algorithm>
#include <functional>
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
// for QA - bails if given category does not exist
struct BailIfInvalid : std::binary_function<std::string, std::string, void>
{
    void operator()(const std::string& cat, const std::string& portdir) const
    {
        if (not util::is_dir(portdir+"/"+cat))
            throw QAException("invalid category '"+cat+"'.");
    }
};

void
Categories::fill()
{
    if (_init)
        return;

    /* read main categories file */
    {
        std::ifstream stream((_portdir+CATEGORIES).c_str());
        if (not stream)
            throw FileException(_portdir+CATEGORIES);

        _s.insert(std::istream_iterator<std::string>(stream),
                  std::istream_iterator<std::string>());
    }

    /* virtual isn't really a category */
    _s.erase("virtual");

    /* validate if requested */
    if (_validate)
        std::for_each(_s.begin(), _s.end(),
            std::bind2nd(BailIfInvalid(), _portdir));

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
