/*
 * herdstat -- portage/developer.cc
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

#include <herdstat/portage/developer.hh>

namespace portage {
/****************************************************************************/
Developer::Developer()
    : _user(), _email(), _name(), _pgpkey(), _joined(), _birth(),
      _status(), _role(), _location(), _awaymsg(), _away(false), _herds()
{
}
/****************************************************************************/
Developer::Developer(const std::string &user, const std::string &email,
                     const std::string &name)
    : _user(user), _email(email.empty() ? _user : email),
      _name(name), _pgpkey(), _joined(), _birth(), _status(),
      _role(), _location(), _awaymsg(), _away(false), _herds()
{
    /* chop everything after '@' if the caller
     * gave an email addy instead of the user name.
     */
    std::string::size_type pos = _user.find('@');
    if (pos != std::string::npos)
        _user.erase(pos);
}
/****************************************************************************/
Developer::~Developer()
{
}
/****************************************************************************/
Developers::Developers()
    : _devs()
{
}
/****************************************************************************/
Developers::Developers(const container_type& v)
    : _devs()
{
    *this = v;
}
/****************************************************************************/
Developers::Developers(const Developers& that)
    : _devs()
{
    *this = that._devs;
}
/****************************************************************************/
Developers::Developers(const std::vector<std::string>& devs)
    : _devs()
{
    *this = devs;
}
/****************************************************************************/
Developers::~Developers()
{
    std::for_each(this->begin(), this->end(), util::DeleteAndNullify());
}
/****************************************************************************/
Developers&
Developers::operator= (const std::vector<std::string>& devs)
{
    std::for_each(this->begin(), this->end(), util::DeleteAndNullify());
    _devs.clear();
    _devs.reserve(devs.size());

    std::vector<std::string>::const_iterator i;
    for (i = devs.begin() ; i != devs.end() ; ++i)
        _devs.push_back(new Developer(*i));

    std::sort(_devs.begin(), _devs.end(), util::DereferenceLess<Developer>());
    return *this;
}
/****************************************************************************/
Developers&
Developers::operator= (const container_type& v)
{
    std::for_each(_devs.begin(), _devs.end(), util::DeleteAndNullify());
    _devs.clear();
    _devs.reserve(v.size());

    for (const_iterator i = v.begin() ; i != v.end() ; ++i)
        _devs.push_back(new Developer(**i));

    std::sort(_devs.begin(), _devs.end(), util::DereferenceLess<Developer>());
    return *this;
}
/****************************************************************************/
Developers::operator
std::vector<std::string>() const
{
    std::vector<std::string> v;
    for (Developers::const_iterator i = this->begin() ; i != this->end() ; ++i)
        v.push_back((*i)->user());
    return v;
}
/****************************************************************************/
} // namespace portage

/* vim: set tw=80 sw=4 et : */
