/*
 * herdstat -- portage/herds_xml.cc
 * $Id: herds_xml.cc 520 2005-09-05 11:59:58Z ka0ttic $
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
#include <herdstat/portage/herds_xml.hh>

namespace {

using namespace portage;

/* any path's found inside a <maintainingproject> tag will be sprintf'd
 * into this for fetching. */
static const char *mpBaseURL = "http://www.gentoo.org/cgi-bin/viewcvs.cgi/*checkout*/xml/htdocs%s?rev=HEAD&root=gentoo&content-type=text/plain";

class mpHandler : public xml::saxhandler
{
    public:
        mpHandler() : in_dev(false) { }
        virtual ~mpHandler() { }

        Herd devs;

    protected:
        virtual bool start_element(const std::string& name,
                                   const attrs_type& attrs);
        virtual bool end_element(const std::string& name);
        virtual bool text(const std::string& text);

    private:
        bool in_dev;
        std::string _cur_role;
};

bool
mpHandler::start_element(const std::string& name, const attrs_type& attrs)
{
    if (name == "dev")
    {
        in_dev = true;
        attrs_type::const_iterator pos = attrs.find("role");
        if (pos != attrs.end())
            _cur_role = pos->second;
    }

    return true;
}

bool
mpHandler::end_element(const std::string& name)
{
    if (name == "dev") in_dev = false;

    return true;
}

bool
mpHandler::text(const std::string& text)
{
    if (in_dev)
    {
        std::string s(util::lowercase(text));
        if (s.find('@') == std::string::npos)
            s.append("@gentoo.org");

        Developer dev(s.substr(0, s.find('@')), s);
        if (not _cur_role.empty())
            dev.set_role(_cur_role);

        devs.push_back(dev);
    }

    return true;
}

} // namespace anonymous

namespace portage {
/*** static members *********************************************************/
const char * const herds_xml::_local_default = LOCALSTATEDIR"/herds.xml";
const char * const herds_xml::_remote_default = "http://www.gentoo.org/cgi-bin/viewcvs.cgi/misc/herds.xml?rev=HEAD;cvsroot=gentoo;content-type=text/plain";
/****************************************************************************/
herds_xml::herds_xml()
    : parsable(), _herds(), _fetched(false), in_herd(false), in_herd_name(false),
      in_herd_email(false), in_herd_desc(false), in_maintainer(false),
      in_maintainer_name(false), in_maintainer_email(false),
      in_maintainer_role(false), in_maintaining_prj(false),
      _cur_herd(), _cur_dev()
{
}
/****************************************************************************/
herds_xml::herds_xml(const std::string& path)
    : parsable(path),
      _herds(), _fetched(false), in_herd(false), in_herd_name(false),
      in_herd_email(false), in_herd_desc(false), in_maintainer(false),
      in_maintainer_name(false), in_maintainer_email(false),
      in_maintainer_role(false), in_maintaining_prj(false),
      _cur_herd(), _cur_dev()
{
    /* FIXME! */
    // this->fetch();
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
    this->set_path(path.empty() ? _local_default : path);
    this->parse_file(this->path().c_str());
}
/****************************************************************************/
void
herds_xml::fetch() const
{
    if (this->_fetched)
        return;



    this->_fetched = true;
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
    {
        _herds.push_back(Herd(text));
        _cur_herd = _herds.find(text);
        assert(_cur_herd != _herds.end());
    }
    else if (in_herd_desc)
        _cur_herd->set_desc(text);
    else if (in_herd_email)
        _cur_herd->set_email(text);        
    else if (in_maintaining_prj)
    {
        /* 
         * special case - for <maintainingproject> we must fetch
         * the listed XML, parse it, and then fill the developer
         * container.
         */

        std::string url(util::sprintf(mpBaseURL, text.c_str()));
        std::string path(std::string(util::dirname(this->path()))+"/"+
                _cur_herd->name()+".xml"); 
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
                return true;
            }
        }

        if (not mps())
            std::cerr << "Failed to save '" << url << "' to" << std::endl
                << "'" << path << "'." << std::endl;
        else
        {
            xml::Document<mpHandler> xml(path);
            mpHandler *handler = xml.handler();

            _cur_herd->insert(_cur_herd->end(),
                    handler->devs.begin() + handler->devs.size() / 2,
                    handler->devs.end());
        }
    }
    else if (in_maintainer_email)
    {
        std::string s(util::lowercase(text));
        _cur_herd->push_back(s);
        _cur_dev = _cur_herd->find(s);
        assert(_cur_dev != _cur_herd->end());
    }
    else if (in_maintainer_name)
        _cur_dev->set_name(_cur_dev->name() + text);
    else if (in_maintainer_role)
        _cur_dev->set_role(text);

    return true;
}
/****************************************************************************/
} // namespace portage

/* vim: set tw=80 sw=4 et : */
