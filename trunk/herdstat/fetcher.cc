/*
 * herdstat -- herdstat/fetcher.cc
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

#include <cstdlib>
#include <unistd.h>
#include <herdstat/util/file.hh>
#include <herdstat/util/string.hh>
#include <herdstat/fetcher.hh>

fetcher::fetcher(const std::string &url,
                 const std::string &path) throw(FetchException)
{
    if (this->fetch(url, path) != EXIT_SUCCESS)
        throw FetchException();
}

void
fetcher::operator() (const std::string &url,
                     const std::string &path) const throw(FetchException)
{
    if (this->fetch(url, path) != EXIT_SUCCESS)
        throw FetchException();
}

int
fetcher::fetch(const std::string &url,
               const std::string &path) const throw(FetchException)
{
    /* ensure we have write access to the directory */
    const char * const dir = util::dirname(path);
    if (access(dir, W_OK) != 0)
        throw FileException(dir);

    /* try to be somewhat safe and remove any occurrences of ';' and '&&'. */
    if (not this->_opts.empty())
    {
        std::string::size_type pos = this->_opts.find(';');
        if (pos != std::string::npos)
            this->_opts.erase(pos);
        if ((pos = this->_opts.find("&&")) != std::string::npos)
            this->_opts.erase(pos);
    }

    return std::system(util::sprintf("%s %s -O %s '%s'", WGET,
            this->_opts.c_str(), path.c_str(), url.c_str()).c_str());
}

/* vim: set tw=80 sw=4 et : */
