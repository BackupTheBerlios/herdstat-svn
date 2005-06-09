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

#include "file.hh"
#include "util_exceptions.hh"

/*****************************************************************************/
void
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
}
/*****************************************************************************/
template <class C>
void
util::base_file_T<C>::open(const char *n, std::ios_base::openmode mode)
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
        throw util::bad_fileobject_E(n);

    this->_opened = true;
}
/*****************************************************************************/
template <class C>
void
util::base_file_T<C>::close()
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
    std::copy(std::istream_iterator<value_type>(*(this->stream)),
        std::istream_iterator<value_type>(), std::back_inserter(*this));
}
/*****************************************************************************/
bool
util::file_T::operator== (const file_T &that) const
{
    if (this->size() != that.size())
        return false;

    /* is every single element equal? */
    std::pair<const_iterator, const_iterator> p;
    p = std::mismatch(this->begin(), this->end(), that.begin());
    return (p.first != this->end());
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
        throw util::errno_E("closedir: " + this->_path);
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
        throw util::bad_fileobject_E(this->_path);

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
util::dir_T::find(const util::path_T &base)
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
util::dir_T::find(const util::path_T &base) const
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
bool
util::is_dir(const char *path)
{
    struct stat s;
    if (stat(path, &s) != 0)
	return false;
    return S_ISDIR(s.st_mode);
}
/*****************************************************************************/
bool util::is_dir(const util::path_T &path) { return is_dir(path.c_str()); }
/*****************************************************************************/
bool util::is_dir(const struct stat &s) { return S_ISDIR(s.st_mode); }
/*****************************************************************************/
bool
util::is_file(const char *path)
{
    struct stat s;
    if (stat(path, &s) != 0)
	return false;
    return S_ISREG(s.st_mode);
}
/*****************************************************************************/
bool util::is_file(const util::path_T &path) { return is_file(path.c_str()); }
/*****************************************************************************/
bool util::is_file(const struct stat &s) { return S_ISREG(s.st_mode); }
/*****************************************************************************/
const char *
util::basename(const char *path)
{
    util::path_T result(path);
    util::path_T::size_type pos;

    /* chop all trailing /'s */
    while (result[result.length() - 1] == '/' and result.length() > 1)
	result.erase(result.length() - 1);

    if ((pos = result.rfind('/')) != util::path_T::npos)
	result = result.substr(pos + 1);

    return ( result.empty() ? "/" : result.c_str() );
}
/*****************************************************************************/
const char *
util::basename(const util::path_T &path) { return util::basename(path.c_str()); }
/*****************************************************************************/
const char *
util::dirname(const char *path)
{
    util::path_T result(path);
    util::path_T::size_type pos;

    /* chop all trailing /'s */
    while (result[result.length() - 1] == '/' and result.length() > 1)
	result.erase(result.length() - 1);

    if ((pos = result.rfind('/')) != util::path_T::npos)
	result = result.substr(0, pos);

    return ( result.empty() ? "/" : result.c_str() );
}
/*****************************************************************************/
const char *
util::dirname(const util::path_T &path) { return util::dirname(path.c_str()); }
/*****************************************************************************/
const char *
util::chop_fileext(const char *path, unsigned short depth)
{
    util::path_T result(path);

    for (; depth > 0 ; --depth)
    {
        util::path_T::size_type pos = result.rfind('.');
        if (pos != util::path_T::npos)
            result = result.substr(0, pos);
    }

    return result.c_str();
}
/*****************************************************************************/
const char *
util::chop_fileext(const util::path_T &path, unsigned short depth)
{
    return util::chop_fileext(path.c_str(), depth);
}
/*****************************************************************************/
void
util::copy_file(const util::path_T &from, const util::path_T &to)
{
    /* remove to if it exists */
    if (util::is_file(to) and (unlink(to.c_str()) != 0))
	throw util::bad_fileobject_E(to);

    const std::auto_ptr<std::ifstream>
        ffrom(new std::ifstream(from.c_str()));
    const std::auto_ptr<std::ofstream>
        fto(new std::ofstream(to.c_str()));

    if (not (*ffrom))
	throw util::bad_fileobject_E(from);
    if (not (*fto))
	throw util::bad_fileobject_E(to);

    /* read from ffrom and write to fto */
    std::string line;
    while (std::getline(*ffrom, line))
	*fto << line << std::endl;
}
/*****************************************************************************/
void
util::move_file(const util::path_T &from, const util::path_T &to)
{
    util::copy_file(from, to);
    if (unlink(from.c_str()) != 0)
	throw util::bad_fileobject_E(from);
}
/*****************************************************************************/

/* vim: set tw=80 sw=4 et : */
