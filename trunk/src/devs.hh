/*
 * herdstat -- src/devs.hh
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

#ifndef HAVE_TYPES_HH
#define HAVE_TYPES_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <fstream>
#include <vector>
#include <string>
#include "formatter.hh"
#include "options.hh"

/*
 * Display developer attributes (name, role, etc).
 */

class dev_attrs_T : public std::vector<std::string>
{
    public:
        void display(std::ostream &stream)
        {
            formatter_T out;

            if (optget("all", bool) and not name.empty())
                out.append("", name);
            else if (not optget("all", bool))
            {
                if (not name.empty())
                    out.append("", name);
                if (not role.empty())
                    out.append("", role);

                for (iterator i = this->begin() ; i != this->end() ; ++i)
                    out.append("", *i);
            }

            out.endl();
        }

        std::string role;
        std::string name;
};

#endif

/* vim: set tw=80 sw=4 et : */
