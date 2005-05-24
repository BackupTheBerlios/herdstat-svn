/*
 * herdstat -- src/devaway.cc
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

#include <memory>
#include "devaway.hh"

#define DEVAWAY_REMOTE      "http://dev.gentoo.org/devaway/"
#define DEVAWAY_EXPIRE      86400

void
devaway_T::init()
{
    struct stat s;

    if ((stat(DEVAWAY_LOCAL, &s) != 0) or
       ((time(NULL) - s.st_mtime) > DEVAWAY_EXPIRE) or (s.st_size == 0))
        this->fetch();

    assert(util::is_file(this->_path));
    this->parse();
}

void
devaway_T::fetch()
{
    struct stat s;

    try
    {
        if (util::is_file(DEVAWAY_LOCAL))
            util::copy_file(DEVAWAY_LOCAL, DEVAWAY_LOCAL".bak");

        if ((util::fetch(DEVAWAY_REMOTE, DEVAWAY_LOCAL) != 0) or
            (stat(DEVAWAY_LOCAL, &s) != 0) or (s.st_size > 0))
            throw fetch_E();

        unlink(DEVAWAY_LOCAL".bak");
    }
    catch (const fetch_E &e)
    {
        if (util::is_file(DEVAWAY_LOCAL".bak"))
            util::move_file(DEVAWAY_LOCAL".bak", DEVAWAY_LOCAL);
        else
        {
            std::cerr << "Failed to fetch " << DEVAWAY_REMOTE << "."
                << std::endl;
            throw;
        }
    }
}

void
devaway_T::parse(const string_type &path)
{
    const std::auto_ptr<std::ifstream>
        f(new std::ifstream(this->_path.c_str()));
    
    if (not (*f))
        throw util::bad_fileobject_E(this->_path);

    std::string s;
    util::string::size_type startpos, endpos;
    while (std::getline(*f, s))
    {
        util::string line(s);

        /* strip leading whitespace */
        if ((startpos = line.find_first_not_of(" \t")) != util::string::npos)
            line.erase(0, startpos);
        /* strip trailing whitespace */
        if ((startpos = line.find_last_of(" \t")) != util::string::npos)
            line.erase(startpos);

        if (line.length() < 1)
            continue;

        util::string dev, awaymsg;
        if ((startpos = line.find_first_of("<tr><td>")) != util::string::npos)
        {
            endpos = line.find_first_of("</td><td>");
            if (endpos == util::string::npos)
                continue;
            
            dev.assign(line.substr(startpos, endpos));
            awaymsg.assign(line.substr(endpos + 9));
        }
        else
            continue;

        this->_away[dev] = awaymsg;
    }
}

/* vim: set tw=80 sw=4 et : */
