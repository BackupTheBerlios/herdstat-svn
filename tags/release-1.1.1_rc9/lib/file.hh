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
                struct stat s;
                return (::stat(this->c_str(), &s) == 0);
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
            { (void)(*this)(); }

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
                (void)(*this)();
            }

            bool exists() const { return this->_exists; }

            virtual bool operator() ();

        protected:
            path_T _path;
            ftype_T _type;
            bool _exists, _opened;
    };

    class base_fileobject_T
    {
        public:
            base_fileobject_T() : _opened(false) { }
            base_fileobject_T(const path_T &path)
                : _path(path), _stat(path), _opened(false) { }

            virtual ~base_fileobject_T() { }

            /* properties */
            bool is_open() const { return this->_opened; }

            virtual void dump(std::ostream &) const { }
            virtual void open()     = 0;
            virtual void close()    = 0;
            virtual void read()     = 0;
            
        protected:
            path_T  _path;
            stat_T  _stat;
            bool    _opened;
    };

    class base_file_T : public base_fileobject_T
    {
        public:
            typedef std::fstream stream_type;

            base_file_T() : stream(NULL) { }
            base_file_T(const path_T &path, std::ios_base::openmode mode = DEFAULT_MODE)
                : base_fileobject_T(path), stream(NULL)
            { this->open(this->_path.c_str(), mode); }

            virtual ~base_file_T() { if (this->_opened) this->close(); }

            virtual void open() { this->open(this->_path.c_str(), DEFAULT_MODE); }
            virtual void open(const char *,
                std::ios_base::openmode mode = DEFAULT_MODE);
            virtual void open(std::ios_base::openmode mode)
            { this->open(this->_path.c_str(), mode); }
            virtual void close();

        protected:
            stream_type *stream;
    };

    class file_T : public base_file_T,
                   public std::vector<string>
    {
        public:
            file_T() { }
            file_T(const path_T &path, std::ios_base::openmode mode = DEFAULT_MODE)
                : base_file_T(path, mode)
            { this->read(); }

            virtual ~file_T() { }

            stat_T::size_type size() const { return this->_stat.size(); }
            size_type bufsize() const
            { return std::vector<string>::size(); }

            virtual bool operator== (const file_T &) const;
            virtual bool operator!= (const file_T &f) const
            { return not (*this == f); }

            virtual void read();
            virtual void dump(std::ostream &) const;
            virtual void write()
            {
                this->dump(*(this->stream));
                this->clear();
            }
    };

    class dir_T  : public base_fileobject_T,
                   public std::vector<path_T>
    {
        public:
            dir_T() : _dirp(NULL) { }
            dir_T(const path_T &path)
                : base_fileobject_T(path), _dirp(NULL)
            { this->open(); this->read(); }

            virtual ~dir_T() { if (this->_opened) this->close(); }

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
