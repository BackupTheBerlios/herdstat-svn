/*
 * herdstat -- lib/file.cc
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

#include <algorithm>
#include <functional>
#include <iterator>
#include <utility>
#include <cstring>
#include <cassert>

#include <herdstat/util/file.hh>
#include <herdstat/exceptions.hh>

/*****************************************************************************/
bool
util::stat_T::operator() ()
{
    if (this->_opened)
    {
        /* TODO: figure out how to fstat an open stream. */
    }
    else
    {
        this->_exists = (::stat(this->_path.c_str(), this) == 0);
    }

    if (S_ISREG(this->st_mode))
        this->_type = REGULAR;
    else if (S_ISDIR(this->st_mode))
        this->_type = DIRECTORY;
    else if (S_ISCHR(this->st_mode))
        this->_type = CHARACTER;
    else if (S_ISBLK(this->st_mode))
        this->_type = BLOCK;
    else if (S_ISFIFO(this->st_mode))
        this->_type = FIFO;
    else if (S_ISLNK(this->st_mode))
        this->_type = LINK;
    else if (S_ISSOCK(this->st_mode))
        this->_type = SOCKET;

    return this->_exists;
}
/*****************************************************************************/
void
util::base_file_T::open(const char *n, std::ios_base::openmode mode)
{
    if (this->_opened)
        return;

    if (this->_path != n)
    {
        this->_path = n;
        this->_stat();
    }

    if (this->stream)
    {
        if (this->stream->is_open())
        {
            this->_opened = true;
            return;
        }
        
        this->stream->open(n, mode);
    }
    else
        this->stream = new std::fstream(n, mode);

    if (not this->stream->is_open())
        throw FileException(n);

    this->_opened = true;
}
/*****************************************************************************/
void
util::base_file_T::close()
{
    if (not this->_opened)
        return;

    delete this->stream;
    this->stream = NULL;

    this->_opened = false;
}
/*****************************************************************************/
void
util::file_T::read()
{
    assert(this->stream and this->stream->is_open());

    std::string line;
    while (std::getline(*(this->stream), line))
        this->push_back(std::string(line));
}
/*****************************************************************************/
bool
util::file_T::operator== (const file_T &that) const
{
    if (this->bufsize() != that.bufsize())
        return false;

    return std::equal(this->begin(), this->end(), that.begin());
}
/*****************************************************************************/
void
util::file_T::dump(std::ostream &os) const
{
    std::copy(this->begin(), this->end(),
        std::ostream_iterator<value_type>(os, "\n"));
}
/*****************************************************************************/
void
util::dir_T::close()
{
    if (not this->_opened)
        return;

#ifdef CLOSEDIR_VOID
    closedir(this->_dirp);
#else /* CLOSEDIR_VOID */
    if (closedir(this->_dirp) != 0)
        throw ErrnoException("closedir: " + this->_path);
#endif /* CLOSEDIR_VOID */

    this->_opened = false;
}
/*****************************************************************************/
void
util::dir_T::open()
{
    if (this->_opened)
        return;

    assert(not this->_path.empty());

    this->_dirp = opendir(this->_path.c_str());
    if (not this->_dirp)
        throw FileException(this->_path);

    this->_opened = true;
}
/*****************************************************************************/
void
util::dir_T::read()
{
    struct dirent *d = NULL;
    while ((d = readdir(this->_dirp)))
    {
        /* skip . and .. */
        if ((std::strcmp(d->d_name, ".") == 0) or
            (std::strcmp(d->d_name, "..") == 0))
            continue;

        this->push_back(this->_path + "/" + d->d_name);
    }
}
/*****************************************************************************/
util::dir_T::iterator
util::dir_T::find(const std::string &base)
{
    return std::find(this->begin(), this->end(),
        this->_path + "/" + base);
}
/*****************************************************************************/
util::dir_T::iterator
util::dir_T::find(const util::regex_T &regex)
{
    return std::find_if(this->begin(), this->end(),
        std::bind1st(util::regexMatch(), &regex));
}
/*****************************************************************************/
util::dir_T::const_iterator
util::dir_T::find(const std::string &base) const
{
    return std::find(this->begin(), this->end(),
        this->_path + "/" + base);
}
/*****************************************************************************/
util::dir_T::const_iterator
util::dir_T::find(const util::regex_T &regex) const
{
    return std::find_if(this->begin(), this->end(),
        std::bind1st(util::regexMatch(), &regex));
}
/*****************************************************************************
 * general purpose file-related functions                                    *
 *****************************************************************************/
const char *
util::basename(const std::string &path)
{
    std::string result(path);
    std::string::size_type pos;

    /* chop all trailing /'s */
    while (result[result.length() - 1] == '/' and result.length() > 1)
	result.erase(result.length() - 1);

    if ((pos = result.rfind('/')) != std::string::npos)
	result = result.substr(pos + 1);

    return ( result.empty() ? "/" : result.c_str() );
}
/*****************************************************************************/
const char *
util::dirname(const std::string &path)
{
    std::string result(path);
    std::string::size_type pos;

    /* chop all trailing /'s */
    while (result[result.length() - 1] == '/' and result.length() > 1)
	result.erase(result.length() - 1);

    if ((pos = result.rfind('/')) != std::string::npos)
	result = result.substr(0, pos);

    return ( result.empty() ? "/" : result.c_str() );
}
/*****************************************************************************/
const char *
util::chop_fileext(const std::string &path, unsigned short depth)
{
    std::string result(path);

    for (; depth > 0 ; --depth)
    {
        std::string::size_type pos = result.rfind('.');
        if (pos != std::string::npos)
            result = result.substr(0, pos);
    }

    return result.c_str();
}
/*****************************************************************************/
void
util::copy_file(const std::string &from, const std::string &to)
{
    /* remove to if it exists */
    if (util::is_file(to) and (unlink(to.c_str()) != 0))
	throw FileException(to);

    std::ifstream ffrom(from.c_str());
    std::ofstream fto(to.c_str());

    if (not ffrom)
	throw FileException(from);
    if (not fto)
	throw FileException(to);

    /* read from ffrom and write to fto */
    std::string line;
    while (std::getline(ffrom, line))
	fto << line << std::endl;
}
/*****************************************************************************/
void
util::move_file(const std::string &from, const std::string &to)
{
    util::copy_file(from, to);
    if (unlink(from.c_str()) != 0)
	throw FileException(from);
}
/*****************************************************************************/

/* vim: set tw=80 sw=4 et : */
