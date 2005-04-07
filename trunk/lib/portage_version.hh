/*
 * herdstat -- lib/portage_version.hh
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

#ifndef HAVE_PORTAGE_VERSION_HH
#define HAVE_PORTAGE_VERSION_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <map>
#include <vector>
#include <string>

#include "file.hh"

namespace portage
{
    class version_string_T
    {
        protected:
            void split_version();

            const std::string _ebuild;             /* abs path of ebuild */
            std::string _cmpstr;                   /* comparison string */
            std::string _verstr;                   /* full version string */
            std::map<std::string, std::string> _v; /* version component map */

        public:
            version_string_T(const char *path) : _ebuild(path),
                _verstr(util::chop_fileext(util::basename(path)))
            { this->split_version(); }
            version_string_T(const std::string &path) : _ebuild(path),
                _verstr(util::chop_fileext(util::basename(path)))
            { this->split_version(); }

            const std::string operator() () const;
            const std::string &operator[] (const std::string &s)
            { return _v[s]; }
    };

    /* vector of version_string_T's - usually used for all versions of a pkg */
    typedef std::vector<portage::version_string_T * > versions_T;
}

#endif

/* vim: set tw=80 sw=4 et : */
