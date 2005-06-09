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
#include <cstring>
#include <cassert>
#include <iostream>
#include <utility>

#include "file.hh"
#include "misc.hh"
#include "util_exceptions.hh"

/*****************************************************************************
 * file_T                                                                    *
 *****************************************************************************/
void
util::file_T::open(const char *n, std::ios_base::openmode mode)
{
    if (this->_opened)
        return;

    if (this->_path != n)
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

    if (not this->stream->is_open())
        throw util::bad_fileobject_E(n);

    this->_opened = true;
}
/*****************************************************************************/
void
util::file_T::read(value_type *v)
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
    value_type::iterator i;
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
    if (this->stream)
    {
        if (this->stream->is_open())
            this->stream->close();
        this->_opened = false;
        delete this->stream;
        this->stream = NULL;
    }
}
/*****************************************************************************/
bool
util::file_T::operator==(const file_T &that)
{
    if (this->_contents.size() != that._contents.size())
        return false;

    /* is every single element equal? */
    std::pair<value_type::iterator, value_type::const_iterator> p =
        std::mismatch(this->_contents.begin(), this->_contents.end(),
            that._contents.begin());
    return (p.first != this->_contents.end());
}
/*****************************************************************************
 * base_dir_T                                                                *
 *****************************************************************************/
template <class C>
void
util::base_dir_T<C>::close()
{
    if (not this->_opened)
        return;

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
    if (this->_opened)
        return;

    assert(not this->_path.empty());
    this->_dir = opendir(this->_path.c_str());

    try
    {
        if (not this->_dir)
            throw util::bad_fileobject_E(this->_path);
    }
    catch (const util::bad_fileobject_E &e)
    {
        std::cerr << e.what() << std::endl;
        throw;
    }

    this->_opened = true;
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
/*****************************************************************************/
util::dir_T::iterator
util::dir_T::find(const util::path_T &base)
{
    return std::find(this->_contents.begin(), this->_contents.end(),
        this->_path + "/" + base);
}
/*****************************************************************************/
util::dir_T::iterator
util::dir_T::find(const util::regex_T &regex)
{
    return std::find_if(this->_contents.begin(), this->_contents.end(),
        std::bind1st(util::regexMatch(), &regex));
}
/*****************************************************************************/
util::dir_T::const_iterator
util::dir_T::find(const util::regex_T &regex) const
{
    return std::find_if(this->_contents.begin(), this->_contents.end(),
        std::bind1st(util::regexMatch(), &regex));
}
/*****************************************************************************/
util::dir_T::const_iterator
util::dir_T::find(const util::path_T &base) const
{
    return std::find(this->_contents.begin(), this->_contents.end(),
        this->_path + "/" + base);
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
