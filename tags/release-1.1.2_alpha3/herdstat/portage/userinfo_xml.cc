/*
 * herdstat -- portage/userinfo_xml.cc
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

#include <herdstat/util/file.hh>
#include <herdstat/portage/userinfo_xml.hh>

namespace portage {
/*** static members *********************************************************/
const char * const userinfo_xml::_local_default = LOCALSTATEDIR"/userinfo.xml";
/****************************************************************************/
userinfo_xml::userinfo_xml()
    : xmlBase(), _devs(), in_user(false), in_firstname(false),
      in_familyname(false), in_pgpkey(false), in_email(false), in_joined(false),
      in_birth(false), in_roles(false), in_status(false), in_location(false),
      _cur_dev()
{
}
/****************************************************************************/
userinfo_xml::userinfo_xml(const std::string& path)
    : xmlBase(path), _devs(), in_user(false),
      in_firstname(false), in_familyname(false), in_pgpkey(false),
      in_email(false), in_joined(false), in_birth(false), in_roles(false),
      in_status(false), in_location(false), _cur_dev()
{
    this->parse();
}
/****************************************************************************/
userinfo_xml::~userinfo_xml()
{
}
/****************************************************************************/
void
userinfo_xml::parse(const std::string& path)
{
    if (not path.empty()) this->set_path(path);
    if (not util::is_file(this->path())) throw FileException(this->path());
    this->parse_file(this->path().c_str());
}
/****************************************************************************/
void
userinfo_xml::fill_developer(Developer& dev) const
{
    assert(not dev.user().empty());
    Herd::const_iterator d = _devs.find(dev);
    if (d != _devs.end())
    {
        if (dev.name().empty() and not d->name().empty())
            dev.set_name(d->name());
//        if (dev.email().empty() and not d->email().empty())
//            dev.set_email(d->email());

        dev.set_status(d->status().empty() ? "Active" : d->status());
        dev.set_location(d->location());
        dev.set_joined(d->joined());
        dev.set_birthday(d->birthday());
        dev.set_role(d->role());
        dev.set_pgpkey(d->pgpkey());
    }
}
/****************************************************************************/
bool
userinfo_xml::start_element(const std::string& name, const attrs_type& attrs)
{
    if (name == "user")
    {
        attrs_type::const_iterator pos = attrs.find("username");
        if (pos == attrs.end())
            throw Exception("<user> tag with no username attribute!");

        Developer dev(pos->second);
        dev.set_status("Active");
        _cur_dev = _devs.insert(dev).first;
        in_user = true;
    }
    else if (name == "firstname")
        in_firstname = true;
    else if (name == "familyname")
        in_familyname = true;
    else if (name == "pgpkey")
        in_pgpkey = true;
    else if (name == "email")
    {
        /* we only care about gentoo.org email addy's */
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
userinfo_xml::end_element(const std::string& name)
{
    if      (name == "user")        in_user = false;
    else if (name == "firstname")   in_firstname = false;
    else if (name == "familyname")  in_familyname = false;
    else if (name == "pgpkey")      in_pgpkey = false;
    else if (name == "email")       in_email = false;
    else if (name == "joined")      in_joined = false;
    else if (name == "birthday")    in_birth = false;
    else if (name == "roles")       in_roles = false;
    else if (name == "status")      in_status = false;
    else if (name == "location")    in_location = false;
    return true;
}
/****************************************************************************/
bool
userinfo_xml::text(const std::string& text)
{
    if (in_firstname)
        const_cast<Developer&>(*_cur_dev).set_name(_cur_dev->name() + text);
    else if (in_familyname)
        const_cast<Developer&>(*_cur_dev).set_name(_cur_dev->name() + " " + text);
    else if (in_pgpkey)
        const_cast<Developer&>(*_cur_dev).set_pgpkey(text);
    else if (in_email)
        const_cast<Developer&>(*_cur_dev).set_email(text);
    else if (in_joined)
        const_cast<Developer&>(*_cur_dev).set_joined(text);
    else if (in_birth)
        const_cast<Developer&>(*_cur_dev).set_birthday(text);
    else if (in_roles)
        const_cast<Developer&>(*_cur_dev).set_role(_cur_dev->role() + text);
    else if (in_status)
        const_cast<Developer&>(*_cur_dev).set_status(text);
    else if (in_location)
        const_cast<Developer&>(*_cur_dev).set_location(_cur_dev->location() + text);

    return true;
}
/****************************************************************************/
} // namespace portage

/* vim: set tw=80 sw=4 et : */
