/*
 * herdstat -- portage/devaway_xml.cc
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

#include <iostream>
#include <herdstat/util/string.hh>
#include <herdstat/util/file.hh>
#include <herdstat/portage/devaway_xml.hh>

#define DEVAWAY_EXPIRE  86400

namespace portage {
/*** static members *********************************************************/
const char * const devaway_xml::_local_default = LOCALSTATEDIR"/devaway.xml";
const char * const devaway_xml::_remote_default = "http://dev.gentoo.org/devaway/xml/index.php";
/****************************************************************************/
devaway_xml::devaway_xml()
    : xmlBase(), _devs(), in_devaway(false),
      in_dev(false), in_reason(false), _cur_dev()
{
}
/****************************************************************************/
devaway_xml::devaway_xml(const std::string &path)
    : xmlBase(path), _devs(), in_devaway(false),
      in_dev(false), in_reason(false), _cur_dev()
{
    this->fetch();
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
    else if (this->path().empty())
        this->set_path(_local_default);

    if (not util::is_file(this->path()))
        throw FileException(this->path());

    this->parse_file(this->path().c_str());

    std::sort(_devs.begin(), _devs.end());
}
/****************************************************************************/
void
devaway_xml::do_fetch(const std::string& path) const throw (FetchException)
{
    if (not path.empty())
        this->set_path(path);
    else if (this->path().empty())
        this->set_path(_local_default);

    util::stat_T devaway(this->path());
    if (devaway.exists() and
        ((std::time(NULL) - devaway.mtime()) < DEVAWAY_EXPIRE) and
        (devaway.size() > 0))
        return;

    _fetch(_remote_default, this->path());
}
/****************************************************************************/
void
devaway_xml::fill_developer(Developer& dev) const
{
    assert(not dev.user().empty());

    Developers::const_iterator d = _devs.find(dev);
    if (d != _devs.end())
    {
        dev.set_away(true);
        dev.set_awaymsg(d->awaymsg());
    }
}
/****************************************************************************/
const std::vector<std::string>
devaway_xml::keys() const
{
    std::vector<std::string> v;
    for (Developers::const_iterator i = _devs.begin() ; i != _devs.end() ; ++i)
    {
        v.push_back(i->user());
        v.push_back(i->email());
    }
    return v;
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
        _cur_dev->set_awaymsg(util::tidy_whitespace(
                    _cur_dev->awaymsg() + text));

    return true;
}
/****************************************************************************/
} // namespace portage

/* vim: set tw=80 sw=4 et : */
