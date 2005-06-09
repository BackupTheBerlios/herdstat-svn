/*
 * herdstat -- lib/file.hh
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

#ifndef HAVE_FILE_HH
#define HAVE_FILE_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <ios>
#include <ostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <cerrno>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include "string.hh"
#include "regex.hh"

#define DEFAULT_MODE    std::ios::in

namespace util
{
    class path_T;

    /* general purpose file-related functions */
    bool is_dir(const char *);
    bool is_dir(const path_T &);
    bool is_dir(const struct stat &);
    bool is_file(const char *);
    bool is_file(const path_T &);
    bool is_file(const struct stat &);
    const char *basename(const char *);
    const char *basename(const path_T &);
    const char *dirname(const char *);
    const char *dirname(const path_T &);
    const char *chop_fileext(const char *, unsigned short depth = 1);
    const char *chop_fileext(const path_T &, unsigned short depth = 1);
    void copy_file(const path_T &, const path_T &);
    void move_file(const path_T &, const path_T &);

    enum ftype_T { REGULAR, DIRECTORY, CHARACTER, BLOCK, FIFO, LINK, SOCKET };

    /* path string */
    class path_T : public string
    {
        public:
            explicit path_T() : string() { }
            path_T(const char *n) : string(n) { }
            path_T(const std::string &n) : string(n) { }
            path_T(const string &n) : string(n) { }

#ifdef UNICODE
            path_T(const Glib::ustring &n) : string(n) { }
#endif

            const char *basename() const
            { return util::basename(this->c_str()); }
            const char *dirname() const
            { return util::dirname(this->c_str()); }

            /* same as util::string::split but with a diff delim */
            std::vector<string>
            split(const string::value_type delim =
                static_cast<string::value_type>('/'))
            { return string::split(delim); }

            bool exists() const
            {
                int r = access(this->c_str(), F_OK);
                return ((r == 0) or (errno != ENOENT));
            }
    };

    /* stat wrapper */
    class stat_T : public stat
    {
        public:
            typedef dev_t   device_type;
            typedef ino_t   inode_type;
            typedef mode_t  mode_type;
            typedef uid_t   uid_type;
            typedef gid_t   gid_type;
            typedef off_t   size_type;
            typedef time_t  time_type;

            stat_T() : _type(REGULAR), _exists(false), _opened(false) { }
            stat_T(const path_T &p, bool opened = false)
                : _path(p), _type(REGULAR), _exists(false), _opened(opened)
            { (*this)(); }

            virtual ~stat_T() { }

            device_type device()   const { return this->st_dev; }
            inode_type  inode()    const { return this->st_ino; }
            mode_type   mode()     const { return this->st_mode; }
            uid_type    uid()      const { return this->st_uid; }
            gid_type    gid()      const { return this->st_gid; }
            size_type   size()     const { return this->st_size; }
            time_type   atime()    const { return this->st_atime; }
            time_type   mtime()    const { return this->st_mtime; }
            time_type   ctime()    const { return this->st_ctime; }
            path_T      path()     const { return this->_path; }
            ftype_T     type()     const { return this->_type; }

            void assign(const path_T &p, bool opened = false)
            {
                this->_opened = opened;
                this->_path.assign(p);
                (*this)();
            }

            virtual void operator() ();

        protected:
            path_T _path;
            ftype_T _type;
            bool _exists, _opened;
    };

    template <class C>
    class base_fileobject_T : public C
    {
        public:
            typedef C base_type;
            typedef typename base_type::iterator iterator;
            typedef typename base_type::const_iterator const_iterator;
            typedef typename base_type::size_type size_type;
            typedef typename base_type::value_type value_type;

            base_fileobject_T() : _opened(false) { }
            base_fileobject_T(const path_T &path)
                : _path(path), _stat(path), _opened(false)
            { this->open(); this->read(); }

            virtual ~base_fileobject_T() { }

            /* properties */
            bool is_open() const { return this->_opened; }

            /* TODO: operator= would be a nice interface for copying files. */

            virtual void dump(std::ostream &) const { }
            virtual void open()     = 0;
            virtual void close()    = 0;
            virtual void read()     = 0;
            
        protected:
            path_T  _path;
            stat_T  _stat;
            bool    _opened;
    };

    template <class C>
    class base_file_T : public base_fileobject_T<C>
    {
        public:
            typedef std::fstream stream_type;
            typedef typename C::size_type size_type;
            typedef typename C::iterator iterator;
            typedef typename C::const_iterator const_iterator;

            base_file_T() : stream(NULL) { }
            base_file_T(const path_T &path)
                : base_fileobject_T<C>(path), stream(NULL) { }

            virtual ~base_file_T() { if (this->_opened) this->close(); }

            virtual void open() { this->open(this->_path.c_str(), DEFAULT_MODE); }
            virtual void open(const char *,
                std::ios_base::openmode mode = DEFAULT_MODE);
            virtual void open(std::ios_base::openmode mode)
            { this->open(this->_path.c_str(), mode); }
            virtual void close();

            stat_T::size_type size() const { return this->_stat.size(); }
            size_type bufsize() const { return this->size(); }

        protected:
            stream_type *stream;
    };

    class file_T : public base_file_T<std::vector<string> >
    {
        public:
            file_T() { }
            file_T(const path_T &path) : base_file_T<base_type>(path) { }

            virtual bool operator== (const file_T &) const;
            virtual bool operator!= (const file_T &f) const
            { return not (*this == f); }

            virtual void read();
            virtual void dump(std::ostream &) const;
            virtual void write() const { this->dump(*(this->stream)); }
    };

    class dir_T  : public base_fileobject_T<std::vector<path_T> >
    {
        public:
            dir_T() : _dirp(NULL) { }
            dir_T(const path_T &path)
                : base_fileobject_T<base_type>(path), _dirp(NULL) { }

            virtual ~dir_T() { }

            virtual void open();
            virtual void close();
            virtual void read();

            virtual iterator find(const value_type &);
            virtual iterator find(const regex_T &);
            virtual const_iterator find(const value_type &) const;
            virtual const_iterator find(const regex_T &) const;

        protected:
            DIR *_dirp;
    };
}

#endif

/* vim: set tw=80 sw=4 et : */
