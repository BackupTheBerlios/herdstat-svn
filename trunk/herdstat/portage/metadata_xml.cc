/*
 * herdstat -- portage/metadata_xml.cc
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

#include <locale>
#include <herdstat/util/string.hh>
#include <herdstat/util/file.hh>
#include <herdstat/portage/metadata_xml.hh>

namespace portage {
/****************************************************************************/
metadata_xml::metadata_xml()
    : parsable(), _data(), in_herd(false), in_maintainer(false),
      in_email(false), in_name(false), in_desc(false), in_longdesc(false),
      in_en_longdesc(false), _cur_dev()
{
}
/****************************************************************************/
metadata_xml::metadata_xml(const std::string& path)
    : parsable(path), _data(), in_herd(false), in_maintainer(false),
      in_email(false), in_name(false), in_desc(false), in_longdesc(false),
      in_en_longdesc(false), _cur_dev()
{
    this->parse();
}
/****************************************************************************/
metadata_xml::~metadata_xml()
{
}
/****************************************************************************/
void
metadata_xml::parse(const std::string& path)
{
    if (not path.empty()) this->set_path(path);
    if (not util::is_file(this->path())) throw FileException(this->path());
    this->parse_file(this->path().c_str());
}
/****************************************************************************/
bool
metadata_xml::start_element(const std::string& name, const attrs_type& attrs)
{
    if (name == "catmetadata")
        _data.set_category(true);
    else if (name == "herd")
        in_herd = true;
    else if (name == "maintainer")
        in_maintainer = true;
    else if (name == "email" and in_maintainer)
        in_email = true;
    else if (name == "name" and in_maintainer)
        in_name = true;
    else if (name == "description")
        in_desc = true;
    else if (name == "longdescription")
    {
        /* determine in_longdesc based on locale */

        std::string value, locale(std::locale("").name());

        attrs_type::const_iterator i = attrs.find("lang");
        if (i != attrs.end())
            value.assign(i->second);

        if (value == "en")
        {
            in_en_longdesc = true;
            return true;
        }

        if (value.empty())
            in_longdesc = true;
        else
        {
            if (value == locale.substr(0, 2))
                in_longdesc = true;
            else if ((locale == "C" or locale == "POSIX") and value == "en")
                in_longdesc = true;
        }
    }

    return true;
}
/****************************************************************************/
bool
metadata_xml::end_element(const std::string& name)
{
    if (name == "herd")
        in_herd = false;
    else if (name == "maintainer")
        in_maintainer = false;
    else if (name == "email" and in_maintainer)
        in_email = false;
    else if (name == "name" and in_maintainer)
        in_name = false;
    else if (name == "description")
        in_desc = false;
    else if (name == "longdescription" and in_en_longdesc)
        in_en_longdesc = false;
    else if (name == "longdescription")
        in_longdesc = false;

    return true;
}
/****************************************************************************/
bool
metadata_xml::text(const std::string& text)
{
    if (in_herd)
        _data._herds.insert(Herd(text));
    else if (in_email)
        _cur_dev = _data._devs.insert(Developer(util::lowercase(text))).first;
    else if (in_name)
        const_cast<Developer&>(*_cur_dev).set_name(_cur_dev->name() + text);
    else if (in_desc)
        const_cast<Developer&>(*_cur_dev).set_role(text);
    else if (in_en_longdesc)
        _data.set_longdesc(_data.longdesc() + text);
    else if (in_longdesc)
        _data.set_longdesc(_data.longdesc() + text);

    return true;
}
/****************************************************************************/
} // namespace portage

/* vim: set tw=80 sw=4 et : */
