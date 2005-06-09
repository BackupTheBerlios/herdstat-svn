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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include "string.hh"

#define DEFAULT_MODE    std::ios::in

namespace util
{
    class path_T;

    /* general purpose file-related functions */
    bool is_dir(const char *);
    bool is_dir(const util::path_T &);
    bool is_dir(const struct stat &);
    bool is_file(const char *);
    bool is_file(const util::path_T &);
    bool is_file(const struct stat &);
    const char *basename(const char *);
    const char *basename(const util::path_T &);
    const char *dirname(const char *);
    const char *dirname(const util::path_T &);
    const char *chop_fileext(const char *, unsigned short depth = 1);
    const char *chop_fileext(const util::path_T &, unsigned short depth = 1);
    void copy_file(const util::path_T &, const util::path_T &);
    void move_file(const util::path_T &, const util::path_T &);

    enum ftype_T { REGULAR, DIRECTORY, CHARACTER, BLOCK, FIFO, LINK, SOCKET };

    /* path string */
    class path_T : public util::string
    {
        public:
            const char *basename() const
            { return util::basename(this->c_str()); }
            const char *dirname() const
            { return util::dirname(this->c_str()); }

            /* same as util::string::split but with a diff delim */
            std::vector<util::string>
            split(const util::string::value_type delim = 
                static_cast<util::string::value_type>('/'))
            { return util::string::split(delim); }

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
            typedef path_T  string_type;

            stat_T() : _type(REGULAR), _exists(false), _opened(false) { }
            stat_T(const util::path_T &p, bool opened = false)
                : _path(p), _type(REGULAR), _exists(false), _opened(opened)
            { (*this)(); }

            virtual ~stat_T() { }

            device_type &device()   const { return this->st_dev; }
            inode_type  &inode()    const { return this->st_ino; }
            mode_type   &mode()     const { return this->st_mode; }
            uid_type    &uid()      const { return this->st_uid; }
            gid_type    &gid()      const { return this->st_gid; }
            size_type   &size()     const { return this->st_size; }
            time_type   &atime()    const { return this->st_atime; }
            time_type   &mtime()    const { return this->st_mtime; }
            time_type   &ctime()    const { return this->st_ctime; }
            string_type &path()     const { return this->_path; }
            ftype_T     &type()     const { return this->_type; }

            void assign(const util::path_T &p, bool opened = false)
            {
                this->_opened = opened;
                this->_path.assign(p);
                (*this)();
            }

            virtual void operator() ();

        protected:
            const util::path_T _path;
            ftype_T _type;
            bool _exists;
            const bool _opened;
    };

    template <class C>
    class base_file_T : public std::vector<C>
    {
        public:
            typedef std::vector<C> base_type;

            base_file_T(ftype_T t) : _type(t), _opened(false) { }
            base_file_T(const path_T &path, ftype_T t)
                : _path(path), _stat(path), _type(t)
                /* FIXME: open first and fstat() */
            { this->_stat(); this->open(); this->read(); }

            virtual ~base_file_T() { if (this->_opened) this->close(); }

            /* properties */
            bool is_open() const { return this->_opened; }
            stat_T::size_type &size() const { return this->_stat.size(); }

            /* comparions */
            virtual bool operator== (const base_file_T<C> &) const;
            virtual bool operator!= (const base_file_T<C> &f) const
            { return not (*this == f); }

            /* actions */
            
            /* TODO: operator= would be a nice interface for copying files. */

            virtual void dump(std::ostream &) const;

            virtual void open()     = 0;
            virtual void close()    = 0;
            virtual void read()     = 0;
            
        protected:
            stat_T  _stat;
            path_T  _path;
            bool    _opened;
    };

    class file_T : public base_file_T<util::string>
    {
        public:
            typedef std::fstream stream_type;

            file_T() : base_type(REGULAR), stream(NULL) { }
            file_T(const path_T &path)
                : base_type(path, REGULAR), stream(NULL) { }

            virtual ~file_T() { }

            virtual void open() { this->open(this->path.c_str(), DEFAULT_MODE); }
            virtual void open(const char *,
                std::ios_base::openmode mode = DEFAULT_MODE);
            virtual void open(std::ios_base::openmode mode)
            { this->open(this->_path.c_str(), mode); }
            virtual void close();
            virtual void read();
            virtual void write() const { this->dump(*(this->stream)); }

        protected:
            stream_type *stream;
    };

    class dir_T  : public base_file_T<util::path_T>
    {
        public:
            dir_T() : base_type(DIRECTORY), _dirp(NULL) { }
            dir_T(const path_T &path)
                : base_type(path, DIRECTORY), _dirp(NULL) { }

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
