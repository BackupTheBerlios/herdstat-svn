/*
 * herdstat -- herdstat/email_address.cc
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

#include <herdstat/exceptions.hh>
#include <herdstat/email_address.hh>
/****************************************************************************/
EmailAddress::EmailAddress()
    : _email(), _user(), _domain()
{
}
/****************************************************************************/
EmailAddress::EmailAddress(const std::string& email)
    : _email(), _user(), _domain()
{
    this->parse(email);
}
/****************************************************************************/
EmailAddress::EmailAddress(const std::string& user, const std::string& domain)
    : _email(user+"@"+domain), _user(user), _domain(domain)
{
}
/****************************************************************************/
EmailAddress::~EmailAddress()
{
}
/****************************************************************************/
bool
EmailAddress::parse(const std::string& email)
{
    std::string::size_type pos = email.find('@');
    if (pos == std::string::npos)
    {
        _user.assign(email);
        return false;
    }

    _user.assign(email.substr(0, pos));
    _domain.assign(email.substr(pos+1));
    _email.assign(email);
    
    return true;
}
/****************************************************************************/
/* vim: set tw=80 sw=4 et : */
