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
#include <iterator>
#include <cstring>
#include <cassert>

#include "util.hh"

/**************************
 * file_T                 *
 **************************/

void
util::file_T::open(const char *n, std::ios_base::openmode mode)
{
    if (_path.empty())
    {
        _path = n;
        this->stat();
    }

    if (stream)
    {
        if (stream->is_open())
            return;

        stream->open(n, mode);
    }
    else
        stream = new std::fstream(n, mode);
}

void
util::file_T::read(std::vector<std::string> *v)
{
    assert(stream);
    assert(stream->is_open());

    std::string line;
    while (std::getline(*stream, line))
        v->push_back(line);
}

void
util::file_T::display(std::ostream &stream)
{
    std::copy(_contents.begin(), _contents.end(),
        std::ostream_iterator<std::string>(stream, "\n"));
}

void
util::file_T::close()
{
    assert(stream);
    stream->close();
    delete stream;
    stream = NULL;
}


/**************************
 * base_dir_T             *
 **************************/

template <class C>
void
util::base_dir_T<C>::close()
{
#ifdef CLOSEDIR_VOID
    closedir(_dir);
#else /* CLOSEDIR_VOID */
    if (closedir(_dir) != 0)
        throw util::errno_E("closedir: " + _path);
#endif /* CLOSEDIR_VOID */
}

template <class C>
void
util::base_dir_T<C>::open()
{
    assert(not _path.empty());
    _dir = opendir(_path.c_str());
    if (not _dir)
        throw util::bad_fileobject_E(_path);
}

template <class C>
void
util::base_dir_T<C>::display(std::ostream &stream)
{
    for (iterator i = _contents.begin() ; i != _contents.end() ; ++i)
        stream << *i << std::endl;
}

/**************************
 * dirobject_T            *
 **************************/

void
util::dirobject_T::read()
{
    struct dirent *d = NULL;
    while ((d = readdir(_dir)))
    {
        /* skip . and .. for obvious reasons */
        if ((std::strcmp(d->d_name, ".") == 0) or
             std::strcmp(d->d_name, "..") == 0)
            continue;

        util::fileobject_T *f = NULL;
        std::string path(_path + "/" + d->d_name);

        if (util::is_dir(path))
        {
            if (_recurse)
                f = new util::dirobject_T(path, _recurse);
            else
                f = new util::fileobject_T(path, FTYPE_DIR);
        }
        else if (util::is_file(path))
            f = new util::file_T(path);
        else
            f = new util::fileobject_T(path, FTYPE_FILE);

        assert(f);
        _contents.push_back(f);
    }
}

void
util::dirobject_T::display(std::ostream &stream)
{
    for (iterator i = _contents.begin() ; i != _contents.end() ; ++i)
    {
        stream << (*i)->name() << ": " << (*i)->size() << "b" << std::endl;

        if ((*i)->type() == FTYPE_DIR)
            (*i)->display(stream);
    }
}

util::dirobject_T::~dirobject_T()
{
    for (iterator i = _contents.begin() ; i != _contents.end() ; ++i)
        delete *i;
}

void
util::dir_T::read()
{
    struct dirent *d = NULL;
    while ((d = readdir(_dir)))
    {
        /* skip . and .. */
        if ((std::strcmp(d->d_name, ".") == 0) or
             std::strcmp(d->d_name, "..") == 0)
            continue;

        _contents.push_back(d->d_name);
    }
}

/******************************************
 * general purpose file-related functions *
 ******************************************/

bool
util::is_dir(const char *path)
{
    struct stat s;
    if (stat(path, &s) != 0)
	return false;
    return S_ISDIR(s.st_mode);
}

bool util::is_dir(const std::string &path) { return is_dir(path.c_str()); }
bool util::is_dir(const struct stat &s) { return S_ISDIR(s.st_mode); }

/*
 * Is the specified path a regular file?
 */

bool
util::is_file(const char *path)
{
    struct stat s;
    if (stat(path, &s) != 0)
	return false;
    return S_ISREG(s.st_mode);
}

bool util::is_file(const std::string &path) { return is_file(path.c_str()); }
bool util::is_file(const struct stat &s) { return S_ISREG(s.st_mode); }

const char *
util::basename(const char *path)
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

const char *
util::basename(std::string const &path) { return util::basename(path.c_str()); }

const char *
util::dirname(const char *path)
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

const char *
util::dirname(std::string const &path) { return util::dirname(path.c_str()); }

const char *
util::chop_fileext(const char *path, unsigned short depth)
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

const char *
util::chop_fileext(const std::string &path, unsigned short depth)
{
    return util::chop_fileext(path.c_str(), depth);
}

/* vim: set tw=80 sw=4 et : */
