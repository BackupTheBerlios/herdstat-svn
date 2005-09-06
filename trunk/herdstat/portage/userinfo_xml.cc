/*
 * herdstat -- portage/userinfo_xml.cc
 * $Id: userinfo_xml.cc 520 2005-09-05 11:59:58Z ka0ttic $
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

#include <herdstat/portage/userinfo_xml.hh>

namespace portage {
/*** static members *********************************************************/
const char * const userinfo_xml::_local_default = LOCALSTATEDIR"/userinfo.xml";
const char * const userinfo_xml::_remote_default = "FIXME";
/****************************************************************************/
userinfo_xml::userinfo_xml()
{
}
/****************************************************************************/
userinfo_xml::userinfo_xml(const std::string& path)
{
    this->fetch();
    this->parse();
}
/****************************************************************************/
userinfo_xml::~userinfo_xml()
{
}
/****************************************************************************/
void
userinfo_xml::fetch() const
{
    if (this->_fetched)
        return;


    this->_fetched = true;
}
/****************************************************************************/
void
userinfo_xml::parse(const std::string& path)
{
    if (not path.empty())
        this->set_path(path);

    this->parse_file(this->path().c_str());
}
/****************************************************************************/
bool
metadata_xml::start_element(const std::string& name, const attrs_type& attrs)
{
    if (name == "userlist")
        in_userlist = true;
    else if (name == "user")
    {
        attrs_type::const_iterator pos = attrs.find("username");
        if (pos == attrs.end())
            throw Exception("<user> tag with no username attribute!");

        Developer dev(pos->second);
        _devs.push_back(dev);
        _cur_dev = _devs.find(dev);
        assert(_cur_dev != _devs.end());

        in_user = true;
    }
    else if (name == "realname")
    {
        attrs_type::const_iterator pos = attrs.find("fullname");
        if (pos == attrs.end())
            throw Exception("<realname> tag with no fullname attribute!");

        _cur_dev->set_name(_cur_dev->name() + pos->second);

        in_realname = true;
    }
    else if (name == "pgpkey")
        in_pgpkey = true;
    else if (name == "email")
    {
        if (attrs.find("gentoo") != attrs.end())
            in_email = true;
    }
    else if (name == "joined")
        in_joined = true;
    else if (name == "birthday")
        in_birth = true;
    else if (name == "status")
        in_status = true;
    else if (name == "roles")
        in_roles = true;
    else if (name == "location")
        in_location = true;

    return true;
}
/****************************************************************************/
bool
metadata_xml::end_element(const std::string& name)
{

    return true;
}
/****************************************************************************/
bool
metadata_xml::text(const std::string& text)
{

    return true;
}
/****************************************************************************/
} // namespace portage

/* vim: set tw=80 sw=4 et : */
