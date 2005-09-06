/*
 * herdstat -- portage/devaway_xml.cc
 * $Id: devaway_xml.cc 520 2005-09-05 11:59:58Z ka0ttic $
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

#include <iostream>
#include <herdstat/util/string.hh>
#include <herdstat/portage/devaway_xml.hh>

namespace portage {
/*** static members *********************************************************/
const char * const devaway_xml::_local_default = LOCALSTATEDIR"/devaway.xml";
const char * const devaway_xml::_remote_default = "FIXME";
/****************************************************************************/
devaway_xml::devaway_xml()
    : parsable(), _devs(), _fetched(false), in_devaway(false),
      in_dev(false), in_reason(false), _cur_dev()
{
}
/****************************************************************************/
devaway_xml::devaway_xml(const std::string &path)
    : parsable(path), _devs(), _fetched(false), in_devaway(false),
      in_dev(false), in_reason(false), _cur_dev()
{
    /* FIXME! */
    // this->fetch();
    this->parse();
}
/****************************************************************************/
devaway_xml::~devaway_xml()
{
}
/****************************************************************************/
void
devaway_xml::parse(const std::string& path)
{
    if (not path.empty())
        this->set_path(path);

    this->parse_file(this->path().c_str());
}
/****************************************************************************/
void
devaway_xml::fetch() const
{
    if (this->_fetched)
        return;


    this->_fetched = true;
}
/****************************************************************************/
bool
devaway_xml::start_element(const std::string& name, const attrs_type& attrs)
{
    if (name == "devaway")
        in_devaway = true;
    else if (name == "dev" and in_devaway)
    {
        attrs_type::const_iterator pos = attrs.find("nick");
        if (pos == attrs.end())
        {
            std::cerr << "<dev> tag with no nick attribute!" << std::endl;
            return false;
        }

        Developer dev(pos->second);
        _devs.push_back(dev);
        _cur_dev = _devs.find(dev);
        assert(_cur_dev != _devs.end());

        in_dev = true;
    }
    else if (name == "reason" and in_dev)
        in_reason = true;

    return true;
}
/****************************************************************************/
bool
devaway_xml::end_element(const std::string& name)
{
    if (name == "devaway")      in_devaway = false;
    else if (name == "dev")     in_dev = false;
    else if (name == "reason")  in_reason = false;

    return true;
}
/****************************************************************************/
bool
devaway_xml::text(const std::string& text)
{
    if (in_reason)
        _cur_dev->set_awaymsg(_cur_dev->awaymsg() + text);

    return true;
}
/****************************************************************************/
} // namespace portage

/* vim: set tw=80 sw=4 et : */
