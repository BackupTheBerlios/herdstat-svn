/*
 * herdstat -- portage/herd.cc
 * $Id: herd.cc 520 2005-09-05 11:59:58Z ka0ttic $
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

#include <herdstat/portage/herd.hh>

namespace portage {
/****************************************************************************/
Herd::Herd(const std::string &name,
           const std::string &email, const std::string &desc)
    : _devs(), _name(name), _email(email.empty() ? name+"@gentoo.org" : email),
      _desc(desc)
{

}
/****************************************************************************/
void
Herd::push_back(const std::string& email)
{
    std::string::size_type pos = email.find('@');
    if (pos == std::string::npos)
        _devs.push_back(Developer(email, email+"@gentoo.org"));
    else
        _devs.push_back(Developer(email.substr(0, pos), email));
}
/****************************************************************************/
void
Herd::set_email(const std::string& email)
{
    std::string::size_type pos = email.find('@');
    _email.assign(pos == std::string::npos ? email+"@gentoo.org" : email);
}
/****************************************************************************/
} // namespace portage

/* vim: set tw=80 sw=4 et : */
