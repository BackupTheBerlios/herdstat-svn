/*
 * herdstat -- portage/herds_xml.cc
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
#include <cassert>
#include <herdstat/util/string.hh>
#include <herdstat/util/file.hh>
#include <herdstat/xml/document.hh>
#include <herdstat/portage/project_xml.hh>
#include <herdstat/portage/herds_xml.hh>

#define HERDSXML_EXPIRE     86400

namespace portage {
/*** static members *********************************************************/
const char * const herds_xml::_local_default = LOCALSTATEDIR"/herds.xml";
/****************************************************************************/
herds_xml::herds_xml()
    : xmlBase(), _herds(), _cvsdir(), _force_fetch(false), _fetch(),
      in_herd(false), in_herd_name(false),
      in_herd_email(false), in_herd_desc(false), in_maintainer(false),
      in_maintainer_name(false), in_maintainer_email(false),
      in_maintainer_role(false), in_maintaining_prj(false),
      _cur_herd(), _cur_dev()
{
}
/****************************************************************************/
herds_xml::herds_xml(const std::string& path)
    : xmlBase(path), _herds(), _cvsdir(), _force_fetch(false), _fetch(),
      in_herd(false), in_herd_name(false), in_herd_email(false),
      in_herd_desc(false), in_maintainer(false), in_maintainer_name(false),
      in_maintainer_email(false), in_maintainer_role(false), in_maintaining_prj(false),
      _cur_herd(), _cur_dev()
{
    this->parse();
}
/****************************************************************************/
herds_xml::~herds_xml()
{
}
/****************************************************************************/
void
herds_xml::parse(const std::string& path)
{
    this->timer().start();

    if      (not path.empty())      this->set_path(path);
    else if (this->path().empty())  this->set_path(_local_default);

    if (not util::is_file(this->path()))
        throw FileException(this->path());

    this->parse_file(this->path().c_str());

    this->timer().stop();
}
/****************************************************************************/
void
herds_xml::fill_developer(Developer& dev) const
{
    /* at least the dev's username needs to be present for searching */
    assert(not dev.user().empty());

    /* for each herd */
    for (Herds::const_iterator h = _herds.begin() ; h != _herds.end() ; ++h)
    {
        /* is the developer in this herd? */
        Herd::const_iterator d = h->find(dev);
        if (d != h->end())
        {
            if (dev.name().empty() and not d->name().empty())
                dev.set_name(d->name());
            dev.set_email(d->email());
            dev.append_herd(h->name());
        }
    }
}
/****************************************************************************/
bool
herds_xml::start_element(const std::string& name, const attrs_type& attrs)
{
    if (name == "herd")
        in_herd = true;
    else if (name == "name" and not in_maintainer)
        in_herd_name = true;
    else if (name == "email" and not in_maintainer)
        in_herd_email = true;
    else if (name == "description" and not in_maintainer)
        in_herd_desc = true;
    else if (name == "maintainer")
        in_maintainer = true;
    else if (name == "email" and in_maintainer)
        in_maintainer_email = true;
    else if (name == "name" and in_maintainer)
        in_maintainer_name = true;
    else if (name == "role")
        in_maintainer_role = true;
    else if (name == "maintainingproject")
        in_maintaining_prj = true;

    return true;
}
/****************************************************************************/
bool
herds_xml::end_element(const std::string& name)
{
    if (name == "herd")
        in_herd = false;
    else if (name == "name" and not in_maintainer)
        in_herd_name = false;
    else if (name == "email" and not in_maintainer)
        in_herd_email = false;
    else if (name == "description" and not in_maintainer)
        in_herd_desc = false;
    else if (name == "maintainer")
        in_maintainer = false;
    else if (name == "email" and in_maintainer)
        in_maintainer_email = false;
    else if (name == "name" and in_maintainer)
        in_maintainer_name = false;
    else if (name == "role")
        in_maintainer_role = false;
    else if (name == "maintainingproject")
        in_maintaining_prj = false;

    return true;
}
/****************************************************************************/
bool
herds_xml::text(const std::string& text)
{
    if (in_herd_name)
        _cur_herd = _herds.insert(Herd(text)).first;
    else if (in_herd_desc)
        const_cast<Herd&>(*_cur_herd).set_desc(text);
    else if (in_herd_email)
        const_cast<Herd&>(*_cur_herd).set_email(text);        
    else if (in_maintainer_email)
        _cur_dev = const_cast<Herd&>(*_cur_herd).insert(
                Developer(util::lowercase(text))).first;
    else if (in_maintainer_name)
        const_cast<Developer&>(*_cur_dev).set_name(_cur_dev->name() + text);
    else if (in_maintainer_role)
        const_cast<Developer&>(*_cur_dev).set_role(text);

    else if (in_maintaining_prj)
    {
        /* 
         * special case - for <maintainingproject> we must fetch
         * the listed XML, parse it, and then fill the developer
         * container.
         */

        project_xml mp(text, _cvsdir, _force_fetch);
        const_cast<Herd&>(*_cur_herd).insert(
                mp.devs().begin(), mp.devs().end());
    }

    return true;
}
/****************************************************************************/
} // namespace portage

/* vim: set tw=80 sw=4 et : */
