/*
 * herdstat -- portage/metadata.cc
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

#include <herdstat/portage/metadata.hh>

namespace portage {
/****************************************************************************/
metadata::metadata() : _pkg(), _longdesc(), _cat(false), _herds(), _devs()
{
}
/****************************************************************************/
metadata::metadata(const std::string &pkg)
    : _pkg(pkg), _longdesc(), _cat((pkg.find('/') == std::string::npos)),
      _herds(), _devs()
{
}
/****************************************************************************/
//metadata::metadata(const metadata& that)
//    : _pkg(), _longdesc(), _cat(), _herds(), _devs()
//{
//    *this = that;
//}
/****************************************************************************/
metadata::~metadata()
{
}
/****************************************************************************/
//metadata&
//metadata::operator= (const metadata& that)
//{
//    _pkg = that._pkg;
//    _longdesc = that._longdesc;
//    _cat = that._cat;
//    _herds = that._herds;
//    _devs = that._devs;
//    return *this;
//}
/****************************************************************************/
} // namespace portage

/* vim: set tw=80 sw=4 et : */
