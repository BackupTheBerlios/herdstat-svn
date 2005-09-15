/*
 * herdstat -- herdstat/portage/project_xml.cc
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
#include <herdstat/portage/project_xml.hh>

namespace portage {
/*** static members *********************************************************/
const char * const project_xml::_baseURL = "http://www.gentoo.org/cgi-bin/viewcvs.cgi/*checkout*/xml/htdocs%s?rev=HEAD&root=gentoo&content-type=text/plain";
const char * const project_xml::_baseLocal = "%s/gentoo/xml/htdocs/%s";
/****************************************************************************/
//project_xml::project_xml()
//    : xmlBase(), fetchable(), _devs(), _cvsdir(""), in_sub(false),
//      in_dev(false), in_task(false), _cur_role()
//{
//}
/****************************************************************************/
project_xml::project_xml(const std::string& path, const std::string& cvsdir,
                         const std::string& herd)
    : xmlBase(), fetchable(), _devs(), _cvsdir(cvsdir), _herd_name(herd), 
      in_sub(false), in_dev(false), in_task(false), _cur_role()
{
    this->fetch(path);
    this->parse();
}
/****************************************************************************/
project_xml::~project_xml()
{
}
/****************************************************************************/
//const char * const project_xml::baseURL() const { return _baseURL; }
//const char * const project_xml::baseLocal() const { return _baseLocal; }
/****************************************************************************/
void
project_xml::do_fetch(const std::string& p) const throw (FetchException)
{
    if (not _cvsdir.empty())
    {
        this->set_path(util::sprintf(_baseLocal, _cvsdir.c_str(), p.c_str()));
        return;
    }

    const std::string url(util::sprintf(_baseURL, p.c_str()));
    const std::string path(util::sprintf("%s/%s.xml", LOCALSTATEDIR,
        _herd_name.c_str()));
    this->set_path(path);

    util::stat_T mps(path);

    try
    {
        if (not mps.exists() or
            (mps.exists() and ((std::time(NULL) - mps.mtime()) > 592200)) or
            (mps.size() == 0))
        {
            if (mps.exists())
                util::copy_file(path, path+".bak");

            _fetch(url, path);

            if (not mps() or (mps.size() == 0))
                throw FetchException();

            unlink((path+".bak").c_str());
        }
    }
    catch (const FetchException)
    {
        if (util::is_file(path+".bak"))
            util::move_file(path+".bak", path);

        if (not mps() or (mps.size() == 0))
        {
            unlink(path.c_str());
            return;
        }
    }

    if (not mps())
        std::cerr << "Failed to save '" << url << "' to" << std::endl
            << "'" << path << "'." << std::endl;
}
/****************************************************************************/
void
project_xml::parse(const std::string& path)
{
    if (not path.empty()) this->set_path(path);
    if (not util::is_file(this->path()))
        throw FileException(this->path());

    this->parse_file(this->path().c_str());
}
/****************************************************************************/
bool
project_xml::start_element(const std::string& name, const attrs_type& attrs)
{
    if (name == "task")
        in_task = true;
    else if (name == "subproject");
    {
        attrs_type::const_iterator pos = attrs.find("inheritmembers");
        if ((pos != attrs.end()) and (pos->second == "yes"))
        {
            if ((pos = attrs.find("ref")) != attrs.end())
            {
                in_sub = true;
                _cur_sub.assign(pos->second);
            }
        }
    }
    if (name == "dev" and not in_task)
    {
        in_dev = true;

        attrs_type::const_iterator pos = attrs.find("description");
        if (pos != attrs.end())
            _cur_role.assign(pos->second);
        else if ((pos = attrs.find("role")) != attrs.end())
            _cur_role.assign(pos->second);
    }

    return true;
}
/****************************************************************************/
bool
project_xml::end_element(const std::string& name)
{
    if (name == "task") in_task = false;
    if (name == "subproject") in_sub = false;
    if (name == "dev") in_dev = false;
    return true;
}
/****************************************************************************/
bool
project_xml::text(const std::string& text)
{
    if (in_sub)
    {
        /* fetch file listed in ref attribute (saved in _cur_sub) */
        project_xml mp(text, _cvsdir, _herd_name);
        const Herd& devs(mp.devs());
        Herd::const_iterator i;
        for (i = devs.begin() ; i != devs.end() ; ++i)
            _devs.insert(**i);
    }
    if (in_dev)
    {
        Developer *dev = new Developer(util::lowercase(text));
        if (not _cur_role.empty())
            dev->set_role(_cur_role);
        _devs.insert(dev);
    }

    return true;
}
/****************************************************************************/
} // namespace portage

/* vim: set tw=80 sw=4 et : */
