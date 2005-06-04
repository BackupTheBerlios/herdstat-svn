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
#include "fetcher.hh"
#include "devaway.hh"

#define DEVAWAY_REMOTE      "http://dev.gentoo.org/devaway/"
#define DEVAWAY_EXPIRE      86400
#define DEVAWAY_USER_TOKEN "<tr><td>"
#define DEVAWAY_MSG_TOKEN   "</td><td>"

void
devaway_T::init()
{
    struct stat s;
    if ((stat(DEVAWAY_LOCAL, &s) != 0) or
       ((time(NULL) - s.st_mtime) > optget("devaway.expire", long))
       or (s.st_size == 0))
        this->_path.assign(DEVAWAY_REMOTE);
}

void
devaway_T::fetch()
{
    if (this->_fetched)
        return;

    struct stat s;

    try
    {
        if (this->_path.find("http://") == util::path_T::npos)
            return;

//        debug_msg("fetching '%s'", DEVAWAY_REMOTE);

        if (util::is_file(DEVAWAY_LOCAL))
            util::copy_file(DEVAWAY_LOCAL, DEVAWAY_LOCAL".bak");

        fetcher_T fetch(DEVAWAY_REMOTE, DEVAWAY_LOCAL);

        if ((stat(DEVAWAY_LOCAL, &s) != 0) or (s.st_size == 0))
            throw fetch_E();

//        debug_msg("fetching succeeded");

        unlink(DEVAWAY_LOCAL".bak");
    }
    catch (const fetch_E &e)
    {
        if (util::is_file(DEVAWAY_LOCAL".bak"))
        {
            util::move_file(DEVAWAY_LOCAL".bak", DEVAWAY_LOCAL);
//            debug_msg("fetching failed, using backup copy");
        }
        else
            std::cerr << "Failed to fetch " << DEVAWAY_REMOTE << "."
                << std::endl;

        if (stat(DEVAWAY_LOCAL, &s) == 0 and s.st_size == 0)
            unlink(DEVAWAY_LOCAL);

        if (optget("action", options_action_T) == action_fetch)
            throw;
    }

    this->_path.assign(DEVAWAY_LOCAL);
    assert(util::is_file(this->_path));
    this->_fetched = true;
}

void
devaway_T::parse(const string_type &path)
{
    const std::auto_ptr<std::ifstream>
        f(new std::ifstream(this->_path.c_str()));
    
    if (not (*f))
        throw util::bad_fileobject_E(this->_path);

    std::string s;
    util::string::size_type beginpos, endpos;
    while (std::getline(*f, s))
    {
        util::string line(s), dev, awaymsg;

        /* strip leading whitespace */
        beginpos = line.find_first_not_of(" \t");
        if (beginpos != util::string::npos)
            line.erase(0, beginpos);

        if (line.length() < 1)
            continue;

        beginpos = line.find(DEVAWAY_USER_TOKEN);
        if (beginpos == util::string::npos)
            continue;

        endpos = line.find('<', beginpos);
        if (endpos == util::string::npos)
            continue;

        dev = line.substr(beginpos+std::strlen(DEVAWAY_USER_TOKEN));
        beginpos = dev.find('<');
        if (beginpos == util::string::npos)
            continue;

        dev = dev.substr(0, beginpos);
//        debug_msg("dev = '%s'", dev.c_str());

        beginpos = line.find(DEVAWAY_MSG_TOKEN);
        if (beginpos == util::string::npos)
            continue;

        if (line.length() < std::strlen(DEVAWAY_MSG_TOKEN))
            continue;

        awaymsg = line.substr(beginpos + std::strlen(DEVAWAY_MSG_TOKEN));
        if (awaymsg == "</td></tr>")
            continue;

//        debug_msg("msg = '%s'", awaymsg.c_str());

        (*this)[dev] = util::unhtmlify(awaymsg);

//        debug_msg("unhtmlified: '%s'", (*this)[dev].c_str());
    }
}

const std::vector<devaway_T::key_type>
devaway_T::keys() const
{
    std::vector<key_type> v;
    for (const_iterator i = this->begin() ; i != this->end() ; ++i)
    {
        v.push_back(i->first);

        if (i->first.find('@') == key_type::npos)
            v.push_back(i->first + "@gentoo.org");
    }

    return v;
}

/* vim: set tw=80 sw=4 et : */
