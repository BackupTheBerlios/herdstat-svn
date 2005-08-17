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

namespace util {
/*****************************************************************************/
stat::stat() : _type(file_type::regular), _exists(false), _opened(false)
{
}
/*****************************************************************************/
stat::stat(const std::string &path, bool opened)
    : _path(path), _type(file_type::regular), _exists(false), _opened(opened)
{
    this->operator()();
}
/*****************************************************************************/
stat::~stat()
{
}
/*****************************************************************************/
void
stat::assign(const std::string &path, bool opened)
{
    this->_opened = opened;
    this->_path.assign(path);
    this->operator()();
}
/*****************************************************************************/
bool
stat::operator() ()
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
        this->_type = file_type::regular;
    else if (S_ISDIR(this->st_mode))
        this->_type = file_type::directory;
    else if (S_ISCHR(this->st_mode))
        this->_type = file_type::character;
    else if (S_ISBLK(this->st_mode))
        this->_type = file_type::block;
    else if (S_ISFIFO(this->st_mode))
        this->_type = file_type::fifo;
    else if (S_ISLNK(this->st_mode))
        this->_type = file_type::link;
    else if (S_ISSOCK(this->st_mode))
        this->_type = file_type::socket;

    return this->_exists;
}
/*****************************************************************************/
base_fileobject::base_fileobject() : _opened(false)
{
}
/*****************************************************************************/
base_fileobject::base_fileobject(const std::string &path)
    : _path(path), _stat(path), _opened(false)
{
}
/*****************************************************************************/
base_file::base_file() : stream(NULL)
{
}
/*****************************************************************************/
base_file::base_file(const std::string &path, std::ios_base::openmode mode)
    : base_fileobject(path), stream(NULL)
{
    this->open(this->_path.c_str(), mode);
}
/*****************************************************************************/
base_file::~base_file()
{
    if (this->_opened)
        this->close();
}
/*****************************************************************************/
void
base_file::open(const char *n, std::ios_base::openmode mode)
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
        throw bad_fileobject_E(n);

    this->_opened = true;
}
/*****************************************************************************/
void
base_file::close()
{
    if (not this->_opened)
        return;

    delete this->stream;
    this->stream = NULL;

    this->_opened = false;
}
/*****************************************************************************/
file::file(const std::string &path, std::ios_base::openmode mode)
    : base_file(path, mode), _contents()
{
    this->read();
}
/*****************************************************************************/
void
file::read()
{
    assert(this->stream and this->stream->is_open());

    std::string line;
    while (std::getline(*(this->stream), line))
        this->push_back(std::string(line));
}
/*****************************************************************************/
void
file::write()
{
    this->dump(*(this->stream));
    this->clear();
}
/*****************************************************************************/
bool
file::operator== (const file &that) const
{
    if (this->bufsize() != that.bufsize())
        return false;

    return std::equal(this->begin(), this->end(), that.begin());
}
/*****************************************************************************/
void
file::dump(std::ostream &os) const
{
    std::copy(this->begin(), this->end(),
        std::ostream_iterator<value_type>(os, "\n"));
}
/*****************************************************************************/
dir::dir() : base_fileobject(), _dirp(NULL), _contents()
{
}
/*****************************************************************************/
dir::dir(const std::string &path)
    : base_fileobject(), _dirp(NULL), _contents()
{
    this->open();
    this->read();
}
/*****************************************************************************/
dir::~dir()
{
    if (this->_opened)
        this->_close();
}
/*****************************************************************************/
void
dir::close()
{
    if (not this->_opened)
        return;

#ifdef CLOSEDIR_VOID
    closedir(this->_dirp);
#else /* CLOSEDIR_VOID */
    if (closedir(this->_dirp) != 0)
        throw errno_E("closedir: " + this->_path);
#endif /* CLOSEDIR_VOID */

    this->_opened = false;
}
/*****************************************************************************/
void
dir::open()
{
    if (this->_opened)
        return;

    assert(not this->_path.empty());

    this->_dirp = opendir(this->_path.c_str());
    if (not this->_dirp)
        throw bad_fileobject_E(this->_path);

    this->_opened = true;
}
/*****************************************************************************/
void
dir::read()
{
    dirent *d = NULL;
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
dir::iterator
dir::find(const std::string &base)
{
    return std::find(this->begin(), this->end(),
        this->_path + "/" + base);
}
/*****************************************************************************/
dir::iterator
dir::find(const regex &regex)
{
    return std::find_if(this->begin(), this->end(),
        std::bind1st(regexMatch(), &regex));
}
/*****************************************************************************/
dir::const_iterator
dir::find(const std::string &base) const
{
    return std::find(this->begin(), this->end(),
        this->_path + "/" + base);
}
/*****************************************************************************/
dir::const_iterator
dir::find(const regex &regex) const
{
    return std::find_if(this->begin(), this->end(),
        std::bind1st(regexMatch(), &regex));
}
/*****************************************************************************
 * general purpose file-related functions                                    *
 *****************************************************************************/
bool
is_dir(const std::string &path)
{
    ::stat s;
    if (::stat(path.c_str(), &s) != 0)
	return false;
    return S_ISDIR(s.st_mode);
}
/*****************************************************************************/
bool is_dir(const struct stat &s) { return S_ISDIR(s.st_mode); }
/*****************************************************************************/
bool
is_file(const std::string &path)
{
    ::stat s;
    if (::stat(path.c_str(), &s) != 0)
	return false;
    return S_ISREG(s.st_mode);
}
/*****************************************************************************/
bool is_file(const struct stat &s) { return S_ISREG(s.st_mode); }
/*****************************************************************************/
const char *
basename(const std::string &path)
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
dirname(const std::string &path)
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
chop_fileext(const std::string &path, unsigned short depth)
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
copy_file(const std::string &from, const std::string &to)
{
    /* remove to if it exists */
    if (is_file(to) and (unlink(to.c_str()) != 0))
	throw bad_fileobject_E(to);

    std::ifstream ffrom(from.c_str());
    std::ofstream fto(to.c_str());

    if (not ffrom)
	throw bad_fileobject_E(from);
    if (not fto)
	throw bad_fileobject_E(to);

    /* read from ffrom and write to fto */
    std::string line;
    while (std::getline(ffrom, line))
	fto << line << std::endl;
}
/*****************************************************************************/
void
move_file(const std::string &from, const std::string &to)
{
    copy_file(from, to);
    if (unlink(from.c_str()) != 0)
	throw bad_fileobject_E(from);
}
/*****************************************************************************/

/* vim: set tw=80 sw=4 et : */
