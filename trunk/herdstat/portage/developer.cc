/*
 * herdstat -- portage/developer.cc
 * $Id: developer.cc 520 2005-09-05 11:59:58Z ka0ttic $
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

#include <herdstat/portage/developer.hh>

namespace portage {
/****************************************************************************/
Developer::Developer()
    : _user(), _email(), _name(), _pgpkey(), _joined(), _birth(),
      _status("active"), _role(), _awaymsg(), _away(false), _herds()
{
}
/****************************************************************************/
Developer::Developer(const std::string &user, const std::string &email,
                     const std::string &name)
    : _user(user), _email(email.empty() ? _user+"@gentoo.org" : email),
      _name(name), _pgpkey(), _joined(), _birth(), _status("active"),
      _role(), _awaymsg(), _away(false), _herds()
{
}
/****************************************************************************/
} // namespace portage

/* vim: set tw=80 sw=4 et : */
