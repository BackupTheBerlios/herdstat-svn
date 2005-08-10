/*
 * herdstat -- src/herds_xml.cc
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

#include "fetcher.hh"
#include "herds_xml.hh"

const herds_xml_T::string_type herds_xml_T::_remote_default =
    "http://www.gentoo.org/cgi-bin/viewcvs.cgi/misc/herds.xml?rev=HEAD;cvsroot=gentoo;content-type=text/plain";

void
herds_xml_T::init()
{
    char *result = NULL;
    const util::stat_T herds_xml(this->_local_default);
    
    if (not optget("herds.xml", util::string).empty())
        this->_path.assign(optget("herds.xml", util::string));
    
    else if ((result = std::getenv("HERDS")))
        this->_path.assign(result);
    
    /* check if previously fetched copy is recent */
    else if (herds_xml.exists() and
            ((std::time(NULL) - herds_xml.mtime()) < HERDS_XML_EXPIRE) and
            (herds_xml.size() > 0))
        this->_path = this->_local_default;
    else
        this->_path = this->_remote_default;
}

void
herds_xml_T::fetch()
{
    if (this->_fetched)
        return;

    try
    {
        if ((optget("action", options_action_T) == action_fetch) and
            (this->_path.find("://") == util::path_T::npos))
            this->_path = util::path_T(_remote_default);

        /* is it a URL? */
        if (this->_path.find("://") != util::path_T::npos)
        {
            if (not optget("quiet", bool))
            {
                std::cout
                    << "Fetching herds.xml..."
                    << std::endl << std::endl;
            }

            /* back up cached copy if it exists so we
             * can use it if fetching fails */
            if (util::is_file(this->_local_default))
                util::copy_file(this->_local_default,
                                this->_local_default+".bak");

            /* fetch it */
            fetcher_T fetch(this->_path, this->_local_default);

            /* because we tell wget to clobber the file, if fetching fails
             * for some reason, it'll truncate the old one - make sure the
             * file is >0 bytes. */
            const util::stat_T herds_xml(this->_local_default);
            if (herds_xml.exists() and (herds_xml.size() > 0))
                this->_path = this->_local_default;
            else
                throw fetch_E();

            /* remove backup copy */
            unlink((this->_local_default+".bak").c_str());
        }
    }
    catch (const fetch_E &e)
    {
        std::cerr << "Error fetching " << this->_path << std::endl << std::endl;

        /* if we have an old cached copy, use it */
        if (util::is_file(this->_local_default+".bak"))
        {
            std::cerr << "Using cached copy... ";
            util::move_file(this->_local_default+".bak",
                            this->_local_default);
            this->_path = this->_local_default;
        }

        std::cerr
            << "If you have a local copy, specify it using -H or by "
            << std::endl << "setting the HERDS environment variable."
            << std::endl;

        const util::stat_T herds_xml(this->_local_default);
        if (not herds_xml.exists())
            throw;
        else if (herds_xml.size() == 0)
        {
            unlink(this->_local_default.c_str());
            throw;
        }
        else
            std::cerr << std::endl;

        if (optget("action", options_action_T) == action_fetch)
            throw;
    }

    this->_fetched = true;
}

/*
 * Given a developer username, return a filled devinfo_T object.
 */

const herds_xml_T::devinfo_T
herds_xml_T::get_dev_info(const string_type &dev) const
{
    devinfo_T info(dev);

    const_iterator e = this->end();
    for (const_iterator h = this->begin() ; h != e ; ++h)
    {
        herd_type::iterator d =
            this->_handler->herds[h->first]->find(dev+"@gentoo.org");
        if (d != this->_handler->herds[h->first]->end())
        {
            info.herds.push_back(h->first);

            if (not d->second->name.empty())
                info.name = d->second->name;
        }
    }

    return info;
}

/* vim: set tw=80 sw=4 et : */
