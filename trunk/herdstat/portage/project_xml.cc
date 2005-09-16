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
std::set<std::string> project_xml::_parsed;
/****************************************************************************/
project_xml::project_xml(const std::string& path, const std::string& cvsdir)
    : xmlBase(), fetchable(), _devs(), _cvsdir(cvsdir), 
      in_sub(false), in_dev(false), in_task(false), _cur_role()
{
    if (_cvsdir.empty())
    {
        std::vector<std::string> parts = util::split(path, '/');
        assert(parts.size() > 1);
        this->set_path(util::sprintf("%s/%s.xml", LOCALSTATEDIR,
            (*(parts.end() - 2)).c_str()));
        this->fetch(path);
    }
    else
    {
        this->set_path(util::sprintf(_baseLocal, _cvsdir.c_str(),
            path.c_str()));
    }

    this->parse();
}
/****************************************************************************/
project_xml::~project_xml()
{
}
/****************************************************************************/
void
project_xml::do_fetch(const std::string& p) const throw (FetchException)
{
    if (not _cvsdir.empty())
        return;

    assert(not p.empty());
    const std::string url(util::sprintf(_baseURL, p.c_str()));
    util::stat_T mps(this->path());

    try
    {
        if (not mps.exists() or
            (mps.exists() and ((std::time(NULL) - mps.mtime()) > 592200)) or
            (mps.size() == 0))
        {
            if (mps.exists())
                util::copy_file(this->path(), this->path()+".bak");

            _fetch(url, this->path());

            if (not mps() or (mps.size() == 0))
                throw FetchException();

            unlink((this->path()+".bak").c_str());
        }
    }
    catch (const FetchException)
    {
        if (util::is_file(this->path()+".bak"))
            util::move_file(this->path()+".bak", this->path());

        if (not mps() or (mps.size() == 0))
        {
            unlink(this->path().c_str());
            return;
        }
    }

    if (not mps())
        std::cerr << "Failed to save '" << url << "' to" << std::endl
            << "'" << this->path() << "'." << std::endl;
}
/****************************************************************************/
void
project_xml::parse(const std::string& path)
{
    if (not path.empty()) this->set_path(path);
    if (not util::is_file(this->path()))
        throw FileException(this->path());

    /* if inserting into the set fails, we've already parsed it.
     * this prevents recursively instantiating this class unneededly. */
    if (not _parsed.insert(this->path()).second)
        return;

    this->parse_file(this->path().c_str());
}
/****************************************************************************/
bool
project_xml::start_element(const std::string& name, const attrs_type& attrs)
{
    if (name == "task")
        in_task = true;
    else if (name == "subproject")
    {
        /*
         * If inheritmembers == "yes", fetch the file listed in the ref attr,
         * and treat it as another projectxml, recursing into ourselves.
         * TODO: should we add some kind of reference counting?
         */

        attrs_type::const_iterator pos = attrs.find("inheritmembers");
        if ((pos != attrs.end()) and (pos->second == "yes"))
        {
            if ((pos = attrs.find("ref")) != attrs.end())
            {
                in_sub = true;

                project_xml mp(pos->second, _cvsdir);
                Herd::const_iterator i;
                for (i = mp.devs().begin() ; i != mp.devs().end() ; ++i)
                {
                    Developer *d = new Developer(**i);
                    Herd::iterator i = _devs.find(d);
                    if (i == _devs.end())
                        _devs.insert(d);
                    else
                    {
                        if (not d->role().empty() and (*i)->role().empty())
                            (*i)->set_role(d->role());
                        delete d;
                    }
                }
            }
        }
    }
    else if (name == "dev" and not in_task)
    {
        in_dev = true;

        attrs_type::const_iterator pos = attrs.find("description");
        if (pos != attrs.end())
            _cur_role.assign(pos->second);
    }

    return true;
}
/****************************************************************************/
bool
project_xml::end_element(const std::string& name)
{
    if (name == "task")             in_task = false;
    else if (name == "subproject")  in_sub = false;
    else if (name == "dev")         in_dev = false;
    return true;
}
/****************************************************************************/
bool
project_xml::text(const std::string& text)
{
    if (in_dev)
    {
        Developer *dev = new Developer(util::lowercase(text));
        if (not _cur_role.empty())
            dev->set_role(_cur_role);

        Herd::iterator i = _devs.find(dev);
        if (i == _devs.end())
            _devs.insert(dev);
        else
        {
            if (not dev->role().empty() and (*i)->role().empty())
                (*i)->set_role(dev->role());
            delete dev;
        }
    }

    return true;
}
/****************************************************************************/
} // namespace portage

/* vim: set tw=80 sw=4 et : */
