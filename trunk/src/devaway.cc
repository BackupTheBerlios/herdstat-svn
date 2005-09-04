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
#include <herdstat/util/string.hh>
#include <herdstat/util/file.hh>
#include "fetcher.hh"
#include "devaway.hh"

#define DEVAWAY_REMOTE      "http://dev.gentoo.org/devaway/"
#define DEVAWAY_EXPIRE      86400
#define DEVAWAY_USER_TOKEN "<tr><td>"
#define DEVAWAY_MSG_TOKEN   "</td><td>"

void
devaway_T::init()
{
    std::string file(optget("devaway.location", std::string));
    if (not file.empty())
        this->_path.assign(file);
    
    util::stat_T devaway(this->path());
    if (not devaway.exists() or
       ((std::time(NULL) - devaway.mtime()) > optget("devaway.expire", long))
       or (devaway.size() == 0))
        this->_path.assign(DEVAWAY_REMOTE);
}

void
devaway_T::fetch()
{
    if (this->_fetched or
       (not optget("devaway.location", std::string).empty()))
        return;

    const std::string path(this->path());
    util::stat_T devaway;

    try
    {
        if (this->_path.find("http://") == std::string::npos)
            return;

        devaway.assign(this->_local);
        if (devaway.exists())
            util::copy_file(this->_local, this->_local+".bak");

        fetcher_T fetch(DEVAWAY_REMOTE, this->_local);

        if (not devaway() or (devaway.size() == 0))
            throw FetchException();

        unlink((this->_local+".bak").c_str());
    }
    catch (const FetchException &e)
    {
        if (util::is_file(this->_local+".bak"))
            util::move_file(this->_local+".bak", this->_local);
        else
            std::cerr << "Failed to fetch " << DEVAWAY_REMOTE << "."
                << std::endl;

        devaway.assign(this->_local);
        if (devaway.exists() and (devaway.size() == 0))
            unlink(this->_local.c_str());

        if (optget("action", options_action_T) == action_fetch)
            throw;
    }

    this->_path.assign(this->_local);
    assert(devaway());
    this->_fetched = true;
}

void
devaway_T::parse(const string_type &path)
{
    const std::auto_ptr<std::ifstream>
        f(new std::ifstream(this->_path.c_str()));
    
    if (not (*f))
        throw FileException(this->_path);

    std::string s;
    std::string::size_type beginpos, endpos;
    while (std::getline(*f, s))
    {
        std::string line(s), dev, awaymsg;

        /* strip leading whitespace */
        beginpos = line.find_first_not_of(" \t");
        if (beginpos != std::string::npos)
            line.erase(0, beginpos);

        if (line.length() < 1)
            continue;

        beginpos = line.find(DEVAWAY_USER_TOKEN);
        if (beginpos == std::string::npos)
            continue;

        endpos = line.find('<', beginpos);
        if (endpos == std::string::npos)
            continue;

        dev = line.substr(beginpos+std::strlen(DEVAWAY_USER_TOKEN));
        beginpos = dev.find('<');
        if (beginpos == std::string::npos)
            continue;

        dev = dev.substr(0, beginpos);
//        debug_msg("dev = '%s'", dev.c_str());

        beginpos = line.find(DEVAWAY_MSG_TOKEN);
        if (beginpos == std::string::npos)
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
