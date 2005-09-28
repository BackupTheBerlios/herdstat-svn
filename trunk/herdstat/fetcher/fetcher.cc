/*
 * herdstat -- herdstat/fetcher/fetcher.cc
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
#include <unistd.h>

#include <herdstat/exceptions.hh>
#include <herdstat/util/file.hh>
#include <herdstat/fetcher/curlfetcher.hh>
#include <herdstat/fetcher/wgetfetcher.hh>
#include <herdstat/fetcher/fetcher.hh>

Fetcher::Fetcher() : _imp(NULL)
{
    newFetcherImp();
}

Fetcher::Fetcher(const FetcherOptions& opts) : _imp(NULL)
{
    newFetcherImp();
    _imp->set_options(opts);
}

Fetcher::Fetcher(const std::string& url, const std::string& path)
    : _imp(NULL)
{
    newFetcherImp();
    this->operator()(url, path);
}

Fetcher::~Fetcher()
{
    if (_imp) delete _imp;
}

void
Fetcher::newFetcherImp()
{
    if (not _imp)
    {
#ifdef FETCH_METHOD_CURL
        _imp = new CurlFetcher;
#else
        _imp = new WgetFetcher;
#endif
    }
}

void
Fetcher::operator()(const std::string& url, const std::string& path)
{
    /* ensure we have write access to the directory */
    const char * const dir = util::dirname(path).c_str();
    if (access(dir, W_OK) != 0)
        throw FileException(dir);

    if (_imp->opts.verbose)
        std::cerr << "Fetching " << url << std::endl;

    if (not _imp->fetch(url, path))
        throw FetchException();
}

/* vim: set tw=80 sw=4 et : */
