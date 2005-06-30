/*
 * herdstat -- src/herds_xml_handler.cc
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

#include <ctime>
#include "common.hh"
#include "fetcher.hh"
#include "xml.hh"
#include "herds_xml_handler.hh"

/* any path's found inside <maintainingproject> will be sprintf'd
 * into this if we need to fetch it */
static const util::string mpBaseURL("http://www.gentoo.org/cgi-bin/viewcvs.cgi/*checkout*/xml/htdocs%s?rev=HEAD&root=gentoo&content-type=text/plain");

/*
 * XML handler for XML file listed in
 * <maintainingproject> elements.
 */

class mpXMLHandler_T : public XMLHandler_T
{
    public:
        typedef herd_T herd_type;
        typedef dev_attrs_T dev_type;

        mpXMLHandler_T() : in_dev(false) { }
        virtual ~mpXMLHandler_T();

        herd_type devs;

    protected:
        virtual return_type
        START_ELEMENT(const string_type &, const attrs_type &);
        virtual return_type END_ELEMENT(const string_type &);
        virtual return_type CHARACTERS(const string_type &);

    private:
        bool in_dev;
        string_type cur_role;
};

mpXMLHandler_T::return_type
mpXMLHandler_T::START_ELEMENT(const string_type &name,
                              const attrs_type &attrs)
{
    if (name == "dev")
    {
        in_dev = true;
        attrs_type::const_iterator pos, pose = attrs.end();
        for (pos = attrs.begin() ; pos != pose ; ++pos)
        {
#ifdef USE_LIBXMLPP
            if (pos->name == "role")
                cur_role = pos->value;
#else /* USE_LIBXMLPP */
            if (pos->first == "role")
                cur_role = pos->second;
#endif /* USE_LIBXMLPP */
        }

    }

#ifdef USE_XMLWRAPP
    return true;
#endif
}

mpXMLHandler_T::return_type
mpXMLHandler_T::END_ELEMENT(const string_type &name)
{
    if (name == "dev")
        in_dev = false;

#ifdef USE_XMLWRAPP
    return true;
#endif
}

mpXMLHandler_T::return_type
mpXMLHandler_T::CHARACTERS(const string_type &str)
{
    if (in_dev)
    {
        string_type s(util::lowercase(str));

        if (str.find('@') == string_type::npos)
            s += "@gentoo.org";

        std::pair<herd_type::iterator, bool> p =
            devs.insert(std::make_pair(s, new dev_type()));
        if (p.second)
            p.first->second->role = cur_role;
    }

#ifdef USE_XMLWRAPP
    return true;
#endif
}

mpXMLHandler_T::~mpXMLHandler_T()
{
    herd_type::iterator i, e = devs.end();
    for (i = devs.begin() ; i != e ; ++i)
        delete i->second;
}

HerdsXMLHandler_T::return_type
HerdsXMLHandler_T::START_ELEMENT(const string_type &name,
                                 const attrs_type &attrs)
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
        in_maintaining_project = true;

#ifdef USE_XMLWRAPP
    return true;
#endif
}

HerdsXMLHandler_T::return_type
HerdsXMLHandler_T::END_ELEMENT(const string_type &name)
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
        in_maintaining_project = false;

#ifdef USE_XMLWRAPP
    return true;
#endif
}

HerdsXMLHandler_T::return_type
HerdsXMLHandler_T::CHARACTERS(const string_type &str)
{
    /* <herd><name> */
    if (in_herd_name)
    {
        cur_herd = str;
        herds[str] = new herd_type(str);
    }

    /* <herd><description> */
    else if (in_herd_desc)
        herds[cur_herd]->desc = str;

    /* <herd><email> */
    else if (in_herd_email)
    {
        /* append @gentoo.org if needed */
        string_type::size_type pos = str.find('@');
        if (pos == string_type::npos)
            herds[cur_herd]->mail = str + "@gentoo.org";
        else if (str.substr(pos) != "@gentoo.org")
            herds[cur_herd]->mail = str.substr(0, pos) + "@gentoo.org";
        else
            herds[cur_herd]->mail = str;
    }

    /* <herd><maintainingproject> */
    else if (in_maintaining_project)
    {
        /* 
         * special case - for <maintainingproject> we must fetch
         * the listed XML, parse it, and then fill the developer
         * container.
         */

        util::string url(util::sprintf(mpBaseURL.c_str(), str.c_str()));
        util::string path(util::string(LOCALSTATEDIR)+"/"+cur_herd+".xml");
        util::stat_T mps(path);

        try
        {
            if (not mps.exists() or
                (mps.exists() and ((std::time(NULL) - mps.mtime()) > 592200)) or
                (mps.size() == 0))
            {
                if (mps.exists())
                    util::copy_file(path, path+".bak");

                fetch(url, path);

                if (not mps() or (mps.size() == 0))
                    throw fetch_E();

                unlink((path+".bak").c_str());
            }
        }
        catch (const fetch_E)
        {
            /* restore back-up copy if it exists and use it */
            if (util::is_file(path+".bak"))
                util::move_file(path+".bak", path);

            if (not mps() or (mps.size() == 0))
            {
                unlink(path.c_str());
#ifdef USE_LIBXMLPP
                return;
#else
                return true;
#endif
            }
        }

        if (not mps())
            std::cerr << "Failed to save '" << url << "' to" << std::endl
                << "'" << path << "'." << std::endl;
        else
        {
            xml_T<mpXMLHandler_T> xml(path);
            mpXMLHandler_T *handler = xml.handler();

            mpXMLHandler_T::herd_type::iterator i, e = handler->devs.end();
            for (i = handler->devs.begin() ; i != e ; ++i)
            {
                std::pair<herd_type::iterator, bool> p =
                    herds[cur_herd]->insert(std::make_pair(i->first, new dev_type()));

                if (p.second)
                    p.first->second->role = i->second->role;

//              debug_msg("p.second = %d\ncur_role = %s", p.second, i->second->role.c_str());
            }
        }
    }

    /* <maintainer><email> */
    else if (in_maintainer_email)
    {
        /* append @gentoo.org if needed */
        string_type::size_type pos = str.find('@');
        if (pos == string_type::npos)
            cur_dev = util::lowercase(str) + "@gentoo.org";
        else if (str.substr(pos) != "@gentoo.org")
            cur_dev = util::lowercase(str.substr(0,pos)) + "@gentoo.org";
        else
            cur_dev = util::lowercase(str);

        herds[cur_herd]->insert(std::make_pair(cur_dev, new dev_type()));
    }

    /* <maintainer><name> */
    else if (in_maintainer_name)
    {
        herd_type::iterator i = herds[cur_herd]->find(cur_dev);
        if (i != herds[cur_herd]->end())
            i->second->name = str;
    }

    /* <maintainer><role> */
    else if (in_maintainer_role)
    {
        herd_type::iterator i = herds[cur_herd]->find(cur_dev);
        if (i != herds[cur_herd]->end())
            i->second->role = str;
    }

#ifdef USE_XMLWRAPP
    return true;
#endif
}

HerdsXMLHandler_T::~HerdsXMLHandler_T()
{
    herds_type::iterator he = herds.end();
    for (herds_type::iterator i = herds.begin() ; i != he ; ++i)
    {
        herd_type::iterator h, e = i->second->end();
        for (h = i->second->begin() ; h != e ; ++h)
            delete h->second;
        delete i->second;
    }
}

/* vim: set tw=80 sw=4 et : */
