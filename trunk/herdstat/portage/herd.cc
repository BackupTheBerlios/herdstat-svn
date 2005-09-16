/*
 * herdstat -- portage/herd.cc
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

#include <herdstat/portage/herd.hh>

namespace portage {
/****************************************************************************/
Herd::Herd()
    : Developers(), _name(), _email(), _desc()
{
}
/****************************************************************************/
Herd::Herd(const std::string &name,
           const std::string &email, const std::string &desc)
    : Developers(), _name(name), _email(email.empty() ? name : email),
      _desc(desc)
{
    /* chop everything after '@' if the caller
     * gave the email addy instead of the herd name.
     */
    std::string::size_type pos = _name.find('@');
    if (pos != std::string::npos)
        _name.erase(pos);
}
/****************************************************************************/
Herd::Herd(const Herd& that)
    : Developers(), _name(), _email(), _desc()
{
    *this = that;
}
/****************************************************************************/
Herd::Herd(const container_type& v)
    : Developers(), _name(), _email(), _desc()
{
}
/****************************************************************************/
Herd::Herd(const std::vector<std::string>& devs)
    : Developers(devs), _name(), _email(), _desc()
{
}
/****************************************************************************/
Herd::~Herd()
{
}
/****************************************************************************/
Herd&
Herd::operator= (const Herd& that)
{
    _name.assign(that._name);
    _email.assign(that._email);
    _desc.assign(that._desc);

    std::for_each(this->begin(), this->end(), util::DeleteAndNullify());
    this->clear();

    for (const_iterator i = that.begin() ; i != that.end() ; ++i)
        this->insert(new Developer(**i));
    
    return *this;
}
/****************************************************************************/
Herds::Herds() : _herds()
{
}
/****************************************************************************/
Herds::Herds(const container_type& v)
    : _herds()
{
    *this = v;
}
/****************************************************************************/
Herds::Herds(const Herds& that)
    : _herds()
{
    *this = that;
}
/****************************************************************************/
Herds::~Herds()
{
    std::for_each(this->begin(), this->end(), util::DeleteAndNullify());
}
/****************************************************************************/
Herds&
Herds::operator= (const std::vector<std::string>& herds)
{
    std::for_each(this->begin(), this->end(), util::DeleteAndNullify());
    _herds.clear();
    std::transform(herds.begin(), herds.end(), std::inserter(_herds, _herds.begin()),
        util::InstantiateStr<Herd>());

    return *this;
}
/****************************************************************************/
Herds&
Herds::operator= (const container_type& v)
{
    std::for_each(_herds.begin(), _herds.end(), util::DeleteAndNullify());
    _herds.clear();
    std::transform(v.begin(), v.end(), std::inserter(_herds, _herds.begin()),
        util::Instantiate<Herd>());

    return *this;
}
/****************************************************************************/
Herds::operator
std::vector<std::string>() const
{
    std::vector<std::string> v;
    for (Herds::const_iterator i = this->begin() ; i != this->end() ; ++i)
        v.push_back((*i)->name());
    return v;
}
/****************************************************************************/
std::pair<Herds::iterator, bool>
Herds::insert(const std::string& herd)
{
    Herd *h = new Herd(herd);
    std::pair<iterator, bool> p = _herds.insert(h);

    if (not p.second)
        delete h;

    return p;
}
/****************************************************************************/
} // namespace portage

/* vim: set tw=80 sw=4 et : */
