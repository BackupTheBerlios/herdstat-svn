/*
 * herdstat -- src/common.cc
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
#include <herdstat/exceptions.hh>
#include <herdstat/fetcher/fetcher.hh>
#include <herdstat/util/string.hh>
#include <herdstat/util/file.hh>

#include "options.hh"
#include "common.hh"

#define HERDSXML_REMOTE "http://www.gentoo.org/cgi-bin/viewcvs.cgi/misc/herds.xml?rev=HEAD;cvsroot=gentoo;content-type=text/plain"
#define HERDSXML_LOCAL  LOCALSTATEDIR"/herds.xml"
#define DEVAWAYXML_REMOTE "http://dev.gentoo.org/devaway/xml/index.php"
#define DEVAWAYXML_LOCAL  LOCALSTATEDIR"/devaway.xml"
#define EXPIRE 86400 /* 24hrs */

using namespace herdstat;

void
debug_msg(const char *fmt, ...)
{
    if (not options::debug())
	return;
    
    va_list v;
    va_start(v, fmt);
    
    std::string s = util::sprintf(fmt, v);

    /* make ASCII colors visible - TODO: anyway to escape them?
     * simply inserting a '\' before it doesnt work... */
    std::string::size_type pos = s.find("\033");
    if (pos != std::string::npos)
	s.erase(pos, 1);

    *(options::outstream()) << "!!! " << s << std::endl;
    va_end(v);
}

static void
do_fetch(const char * const url, const char * const file)
{
    util::Stat xml(file);
    const std::time_t now(std::time(NULL));
    
    /* check if previously fetched copy is expired */
    if ((options::action() != action_fetch) and
        (now != static_cast<std::time_t>(-1)) and xml.exists() and
        ((now - xml.mtime()) < EXPIRE) and (xml.size() > 0))
        return;
    /* exists but expired */
    else if (xml.exists() and (xml.size() > 0))
        /* back it up in case fetching fais */
        util::copy_file(xml.path(), xml.path()+".bak");

    try
    {
        FetcherOptions fetchopts;
        fetchopts.set_verbose(options::verbose());
        fetchopts.set_debug(options::debug());

        Fetcher fetch(fetchopts);
            
        fetch(url, xml.path());

        if (not xml() or (xml.size() == 0))
            throw FetchException();

        /* remove backup */
        unlink((xml.path()+".bak").c_str());
    }
    catch (const FetchException& e)
    {
        std::cerr << "Error fetching " << url << std::endl;

        if (util::is_file(xml.path()+".bak"))
        {
            std::cerr << "Using cached copy..." << std::endl;
            util::move_file(xml.path()+".bak", xml.path());
        }

        if (not xml())
            throw;
        else if (xml.size() == 0)
        {
            unlink(xml.path().c_str());
            throw;
        }
    }
}

void
fetch_devawayxml()
{
    if (options::devawayxml().empty())
        do_fetch(DEVAWAYXML_REMOTE, DEVAWAYXML_LOCAL);
}

void
fetch_herdsxml()
{
    if (options::herdsxml().empty())
        do_fetch(HERDSXML_REMOTE, HERDSXML_LOCAL);
}

/* vim: set tw=80 sw=4 et : */
