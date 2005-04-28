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

#include "herds_xml.hh"

const util::string herds_xml_T::_default =
    "http://www.gentoo.org/cgi-bin/viewcvs.cgi/misc/herds.xml?rev=HEAD;cvsroot=gentoo;content-type=text/plain";

void
herds_xml_T::init()
{
    char *result = NULL;
    struct stat s;
    
    if (not optget("herds.xml", util::string).empty())
        this->_path.assign(optget("herds.xml", util::string));
    
    else if ((result = std::getenv("HERDS")))
        this->_path.assign(result);
    
    /* check if previously fetched copy is recent */
    else if ((stat(HERDS_XML_FETCH_LOCATION, &s) == 0) and
            ((time(NULL) - s.st_mtime) < HERDS_XML_EXPIRE) and (s.st_size > 0))
        this->_path = util::path_T(HERDS_XML_FETCH_LOCATION);

    this->fetch();

    if (this->_fetchonly)
        return;

    if (not this->_path.exists())
        throw bad_fileobject_E(this->_path);

    this->parse();
}

void
herds_xml_T::fetch()
{
    struct stat s;
    try
    {
        if (optget("fetch", bool) and
            (this->_path.find("://") == util::path_T::npos))
            this->_path = util::path_T(_default);

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
            if (util::is_file(HERDS_XML_FETCH_LOCATION))
                util::copy_file(HERDS_XML_FETCH_LOCATION,
                                HERDS_XML_FETCH_LOCATION".bak");

            /* fetch it */
            if (util::fetch(this->_path, HERDS_XML_FETCH_LOCATION,
                optget("verbose", bool)) != 0)
                throw fetch_E();

            /* because we tell wget to clobber the file, if fetching fails
             * for some reason, it'll truncate the old one - make sure the
             * file is >0 bytes. */
            if ((stat(HERDS_XML_FETCH_LOCATION, &s) == 0) and (s.st_size > 0))
                this->_path = util::path_T(HERDS_XML_FETCH_LOCATION);
            else
                throw fetch_E();

            /* remove backup copy */
            unlink(HERDS_XML_FETCH_LOCATION".bak");
        }
    }
    catch (const fetch_E &e)
    {
        std::cerr << "Error fetching " << this->_path << std::endl << std::endl;

        /* if we have an old cached copy, use it */
        if (util::is_file(HERDS_XML_FETCH_LOCATION".bak"))
        {
            std::cerr << "Using cached copy... ";
            util::move_file(HERDS_XML_FETCH_LOCATION".bak",
                            HERDS_XML_FETCH_LOCATION);
            this->_path = HERDS_XML_FETCH_LOCATION;
        }

        std::cerr
            << "If you have a local copy, specify it using -H or by "
            << std::endl << "setting the HERDS environment variable."
            << std::endl;

        if (stat(HERDS_XML_FETCH_LOCATION, &s) != 0)
            throw;
        else if (s.st_size == 0)
        {
            unlink(HERDS_XML_FETCH_LOCATION);
            throw;
        }
        else
            std::cerr << std::endl;
    }
}

void
herds_xml_T::parse()
{
    XMLParser_T parser(&(*(this->_handler)));

    if (optget("timer", bool))
        this->_timer.start();

    parser.parse(this->_path);

    if (optget("timer", bool))
        this->_timer.stop();
}

/* vim: set tw=80 sw=4 et : */
