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
#include <herdstat/portage/herds_xml.hh>

#define HERDSXML_EXPIRE     86400

namespace {

using namespace portage;

/* any path's found inside a <maintainingproject> tag will be sprintf'd
 * into this for fetching. */
static const char *mpBaseURL = "http://www.gentoo.org/cgi-bin/viewcvs.cgi/*checkout*/xml/htdocs%s?rev=HEAD&root=gentoo&content-type=text/plain";
static const char *mpBaseLocal = "%s/gentoo/xml/htdocs/%s";

class mpHandler : public xml::saxhandler
{
    public:
        mpHandler() : devs(), in_dev(false), _cur_role() { }
        virtual ~mpHandler();

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

mpHandler::~mpHandler()
{
}

bool
mpHandler::start_element(const std::string& name, const attrs_type& attrs)
{
    if (name == "dev")
    {
        in_dev = true;
        attrs_type::const_iterator pos = attrs.find("description");
        if (pos != attrs.end())
            _cur_role = pos->second;
        else if ((pos = attrs.find("role")) != attrs.end())
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
        Developer *dev = new Developer(util::lowercase(text));
        if (not _cur_role.empty())
            dev->set_role(_cur_role);

        devs.insert(dev);
    }

    return true;
}

/* parse XML file listed in a <maintainingproject> tag */
static void parse_mp_xml(Herd * const herd, const std::string& path)
{
    xml::Document<mpHandler> xml(path);
    mpHandler *handler = xml.handler();
            
    Herd::iterator i;
    for (i = handler->devs.begin() ; i != handler->devs.end() ; ++i)
        herd->insert(new Developer(**i));
}

} // namespace anonymous

namespace portage {
/*** static members *********************************************************/
const char * const herds_xml::_local_default = LOCALSTATEDIR"/herds.xml";
/****************************************************************************/
herds_xml::herds_xml()
    : xmlBase(), _herds(), _cvsdir(), _fetch(), in_herd(false), in_herd_name(false),
      in_herd_email(false), in_herd_desc(false), in_maintainer(false),
      in_maintainer_name(false), in_maintainer_email(false),
      in_maintainer_role(false), in_maintaining_prj(false),
      _cur_herd(), _cur_dev()
{
}
/****************************************************************************/
herds_xml::herds_xml(const std::string& path)
    : xmlBase(path), _herds(), _cvsdir(), _fetch(), in_herd(false),
      in_herd_name(false), in_herd_email(false), in_herd_desc(false),
      in_maintainer(false), in_maintainer_name(false), in_maintainer_email(false),
      in_maintainer_role(false), in_maintaining_prj(false),
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
        Herd::const_iterator d = (*h)->find(dev.user());
        if (d != (*h)->end())
        {
            if (dev.name().empty() and not (*d)->name().empty())
                dev.set_name((*d)->name());
            dev.set_email((*d)->email());
            dev.append_herd((*h)->name());
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
    {
        std::pair<Herds::iterator, bool> p = _herds.insert(new Herd(text));
        _cur_herd = p.first;
    }
    else if (in_herd_desc)
        (*_cur_herd)->set_desc(util::tidy_whitespace(text));
    else if (in_herd_email)
        (*_cur_herd)->set_email(text);        
    else if (in_maintainer_email)
    {
        std::pair<Herd::iterator, bool> p =
            (*_cur_herd)->insert(new Developer(util::lowercase(text)));
        _cur_dev = p.first;
    }
    else if (in_maintainer_name)
        (*_cur_dev)->set_name((*_cur_dev)->name() + text);
    else if (in_maintainer_role)
        (*_cur_dev)->set_role(text);

    else if (in_maintaining_prj)
    {
        /* 
         * special case - for <maintainingproject> we must fetch
         * the listed XML, parse it, and then fill the developer
         * container.
         */

        if (_cvsdir.empty())
        {
            const std::string path(util::sprintf("%s/%s.xml",
                    LOCALSTATEDIR, (*_cur_herd)->name().c_str()));
            const std::string url(util::sprintf(mpBaseURL, text.c_str()));
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
                parse_mp_xml(*_cur_herd, path);
        }
        else
        {
            const std::string cvspath(
                util::sprintf(mpBaseLocal, _cvsdir.c_str(), text.c_str()));
            if (util::is_file(cvspath))
                parse_mp_xml(*_cur_herd, cvspath);
        }
    }

    return true;
}
/****************************************************************************/
} // namespace portage

/* vim: set tw=80 sw=4 et : */
