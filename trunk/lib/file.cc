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

/*****************************************************************************
 * file_T                                                                    *
 *****************************************************************************/
void
util::file_T::open(const char *n, std::ios_base::openmode mode)
{
    if (this->_path.empty())
    {
        this->_path = n;
        this->stat();
    }

    if (this->stream)
    {
        if (this->stream->is_open())
            return;

        this->stream->open(n, mode);
    }
    else
        this->stream = new std::fstream(n, mode);
}
/*****************************************************************************/
void
util::file_T::read(std::vector<util::string> *v)
{
    assert(this->stream);
    assert(this->stream->is_open());

    std::string line;
    while (std::getline(*(this->stream), line))
        v->push_back(util::string(line));
}
/*****************************************************************************/
void
util::file_T::display(std::ostream &stream)
{
    std::vector<util::string>::iterator i;
    for (i = this->_contents.begin() ; i != this->_contents.end() ; ++i)
        stream << *i << std::endl;

    /* for some reason, the below statement only copies some
     * of the vector....
     */

//    std::copy(this->_contents.begin(), this->_contents.end(),
//        std::ostream_iterator<util::string>(stream, "\n"));
}
/*****************************************************************************/
void
util::file_T::close()
{
    assert(this->stream);
    this->stream->close();
    delete this->stream;
    this->stream = NULL;
}
/*****************************************************************************
 * base_dir_T                                                                *
 *****************************************************************************/
template <class C>
void
util::base_dir_T<C>::close()
{
#ifdef CLOSEDIR_VOID
    closedir(this->_dir);
#else /* CLOSEDIR_VOID */
    if (closedir(this->_dir) != 0)
        throw util::errno_E("closedir: " + this->_path);
#endif /* CLOSEDIR_VOID */
}
/*****************************************************************************/
template <class C>
void
util::base_dir_T<C>::open()
{
    assert(not this->_path.empty());
    this->_dir = opendir(this->_path.c_str());
    if (not this->_dir)
        throw util::bad_fileobject_E(this->_path);
}
/*****************************************************************************/
template <class C>
void
util::base_dir_T<C>::display(std::ostream &stream)
{
    iterator i;
    for (i = this->_contents.begin() ; i != this->_contents.end() ; ++i)
        stream << *i << std::endl;
}
/*****************************************************************************
 * dirobject_T                                                               *
 *****************************************************************************/
void
util::dirobject_T::read()
{
    struct dirent *d = NULL;
    while ((d = readdir(this->_dir)))
    {
        /* skip . and .. for obvious reasons */
        if ((std::strcmp(d->d_name, ".") == 0) or
             std::strcmp(d->d_name, "..") == 0)
            continue;

        util::fileobject_T *f = NULL;
        util::path_T path(this->_path + "/" + d->d_name);

        if (util::is_dir(path))
        {
            if (this->_recurse)
                f = new util::dirobject_T(path, this->_recurse);
            else
                f = new util::fileobject_T(path, FTYPE_DIR);
        }
        else if (util::is_file(path))
            f = new util::file_T(path);
        else
            f = new util::fileobject_T(path, FTYPE_FILE);

        assert(f);
        this->_contents.push_back(f);
    }
}
/*****************************************************************************/
void
util::dirobject_T::display(std::ostream &stream)
{
    iterator i;
    for (i = this->_contents.begin() ; i != this->_contents.end() ; ++i)
    {
        stream << (*i)->name() << ": " << (*i)->size() << "b" << std::endl;

        if ((*i)->type() == FTYPE_DIR)
            (*i)->display(stream);
    }
}
/*****************************************************************************/
util::dirobject_T::~dirobject_T()
{
    iterator i;
    for (i = this->_contents.begin() ; i != this->_contents.end() ; ++i)
        delete *i;
}
/*****************************************************************************/
void
util::dir_T::read()
{
    struct dirent *d = NULL;
    while ((d = readdir(this->_dir)))
    {
        /* skip . and .. */
        if ((std::strcmp(d->d_name, ".") == 0) or
             std::strcmp(d->d_name, "..") == 0)
            continue;

        this->_contents.push_back(this->_path + "/" + d->d_name);
    }
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

    std::auto_ptr<std::ifstream> ffrom(new std::ifstream(from.c_str()));
    std::auto_ptr<std::ofstream> fto(new std::ofstream(to.c_str()));

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
