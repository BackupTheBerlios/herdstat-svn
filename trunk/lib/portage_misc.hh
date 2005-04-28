/*
 * herdstat -- lib/portage_misc.hh
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

#ifndef HAVE_PORTAGE_MISC_HH
#define HAVE_PORTAGE_MISC_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "file.hh"
#include "portage_config.hh"

namespace portage
{
    bool in_pkg_dir();
    bool is_pkg_dir(const util::path_T &);
    bool is_ebuild(const util::path_T &);

    /* represents a list of package categories */
    class categories_T : public util::file_T
    {
        public:
            categories_T()
                : file_T(util::path_T(portage::portdir()) + "/profiles/categories")
            { this->init(); }
            categories_T(const util::path_T &p) : file_T(p + "/profiles/categories")
            { this->init(); }

            void init()
            {
                this->open();
                this->read();

                /* remove 'virtual' ... */
                if (this->_contents.size() > 0 and
                    this->_contents.back() == "virtual")
                    this->_contents.erase(this->_contents.end());
            }

            size_type size() const { return this->_contents.size(); }
    };
}

#endif

/* vim: set tw=80 sw=4 et : */
