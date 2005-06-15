/*
 * herdstat -- lib/portage_misc.cc
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

#include "misc.hh"
#include "file.hh"
#include "portage_exceptions.hh"
#include "portage_misc.hh"

bool portage::categories_T::_init = false;
portage::categories_T::value_type portage::categories_T::_s;

/*****************************************************************************
 * Current working directory a package directory?                            *
 *****************************************************************************/
bool portage::in_pkg_dir() { return portage::is_pkg_dir(util::getcwd()); }
/*****************************************************************************
 * Given path a package directory?                                           *
 *****************************************************************************/
bool
portage::is_pkg_dir(const util::path_T &path)
{
    if (not util::is_dir(path))
        return false;

    const util::regex_T regex("\\.ebuild$");
    const util::dir_T dir(path);
    return dir.find(regex) != dir.end();
}
/*****************************************************************************
 * Is the given path an ebuild?                                              *
 *****************************************************************************/
bool
portage::is_ebuild(const util::path_T &path)
{
    return ( (path.length() > 7) and
             (path.substr(path.length() - 7) == ".ebuild") );
}
/*****************************************************************************/
void
portage::categories_T::init()
{
    if (this->_init)
        return;

    /* read categories from real PORTDIR */
    std::string line;
    {
        std::ifstream stream((this->_portdir + CATEGORIES).c_str());
        if (not stream.is_open())
            throw util::bad_fileobject_E(this->_portdir + CATEGORIES);

        std::size_t n = 0;
        while (std::getline(stream, line))
        {
            /* virtual isn't a real category */
            if (line == "virtual")
                continue;

            /* choke if validate mode is enabled */
            if (this->_validate and ++n and 
                not util::is_dir(this->_portdir + "/" + line))
            {
                std::cerr << "QA Violation: " << this->_portdir
                    << CATEGORIES << ":" << n << std::endl
                    << "category '" << line << "' is listed but does not exist."
                    << std::endl;
                throw portage::qa_E();
            }

            this->_s.insert(line);
        }
    }

    /* read user category file */
    if (util::is_file(CATEGORIES_USER))
    {
        std::ifstream stream(CATEGORIES_USER);
        if (not stream.is_open())
            throw util::bad_fileobject_E(CATEGORIES_USER);

        while (std::getline(stream, line))
            this->_s.insert(line);
    }

    this->_init = true;
}
/*****************************************************************************/

/* vim: set tw=80 sw=4 et : */
