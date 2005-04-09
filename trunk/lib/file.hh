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
#include <fstream>
#include <ostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#define DEFAULT_MODE std::ios::in

namespace util
{
    /* general purpose file-related functions */
    bool is_dir(const char *);
    bool is_dir(const std::string &);
    bool is_dir(const struct stat &);
    bool is_file(const char *);
    bool is_file(const std::string &);
    bool is_file(const struct stat &);
    const char *basename(const char *);
    const char *basename(std::string const &);
    const char *dirname(const char *);
    const char *dirname(std::string const &);
    const char *chop_fileext(const char *, unsigned short depth = 1);
    const char *chop_fileext(const std::string &, unsigned short depth = 1);

    enum type_T { FTYPE_FILE, FTYPE_DIR };

    /* Represents a path to a file object */
    class path_T : public std::string
    {
        public:
            explicit path_T() : std::string() { }
            path_T(const char *n) : std::string(n) { }
            path_T(const std::string &n) : std::string(n) { }

            const std::string basename() const { return util::basename(*this); }
            const std::string dirname() const { return util::dirname(*this); }
    };

    /* generic file object */
    class fileobject_T
    {
        protected:
            path_T _path;        /* path object */
            struct stat _sbuf;   /* stat structure */
            type_T _type;
            bool _exists;

        public:
            typedef off_t size_type;
            typedef time_t time_type;
            typedef mode_t mode_type;
            typedef uid_t uid_type;
            typedef gid_t gid_type;
            typedef dev_t device_type;
            typedef ino_t inode_type;
            typedef blksize_t blksize_type;
            typedef blkcnt_t blkcnt_type;

            fileobject_T(type_T t) : _type(t) { }
            fileobject_T(const char *n, type_T t) : _path(n), _type(t)
            { this->stat(); }
            fileobject_T(const std::string &n, type_T t) : _path(n), _type(t)
            { this->stat(); }
            virtual ~fileobject_T() { }

            size_type size() const { return _sbuf.st_size; }
            time_type mtime() const { return _sbuf.st_mtime; }
            time_type atime() const { return _sbuf.st_atime; }
            time_type ctime() const { return _sbuf.st_ctime; }
            uid_type uid() const { return _sbuf.st_uid; }
            gid_type gid() const { return _sbuf.st_gid; }
            mode_type mode() const { return _sbuf.st_mode; }
            inode_type inode() const { return _sbuf.st_ino; }
            device_type device() const { return _sbuf.st_dev; }
            blksize_type blksize() const { return _sbuf.st_blksize; }
            blkcnt_type blkcnt() const { return _sbuf.st_blocks; }

            std::string &name() { return _path; }
            std::string basename() const { return _path.basename(); }
            std::string dirname() const { return _path.dirname(); }
            type_T type() const { return _type; }
            bool exists() const { return _exists; }

            void stat()
            {
                _exists = ( ::stat(_path.c_str(), &_sbuf) == 0 ? true : false );
            }

            virtual void display(std::ostream &) { }
            virtual void open() { }
            virtual void read() { }
            virtual void close() { }
    };

    /* represents a regular file */
    class file_T : public fileobject_T
    {
        protected:
            std::fstream *stream;
            std::vector<std::string> _contents;

        public:
            typedef std::vector<std::string>::iterator iterator;
            typedef std::vector<std::string>::size_type size_type;

            file_T() : fileobject_T(FTYPE_FILE), stream(NULL) { }
            file_T(const std::string &n)
                : fileobject_T(n, FTYPE_FILE), stream(NULL) { }
            file_T(const char *n)
                : fileobject_T(n, FTYPE_FILE), stream(NULL) { }
            file_T(const std::string &n, std::fstream *s)
                : fileobject_T(n, FTYPE_FILE), stream(s) { }
            file_T(const char *n, std::fstream *s)
                : fileobject_T(n, FTYPE_FILE), stream(s) { }
            virtual ~file_T() { if (stream) delete stream; }

            iterator begin() { return _contents.begin(); }
            iterator end() { return _contents.end(); }
            size_type bufsize() const { return _contents.size(); }
            void push_back(const std::string &s) { _contents.push_back(s); }

            virtual void open() { this->open(_path.c_str(), DEFAULT_MODE); }
            virtual void open(const char *n,
                std::ios_base::openmode mode = DEFAULT_MODE);
            virtual void open(std::ios_base::openmode mode)
            { this->open(_path.c_str(), mode); }

            virtual void close();

            virtual void read() { this->read(&_contents); }
            virtual void read(std::vector<std::string> *);

            virtual void write() { this->display(*stream); }
            virtual void write(const std::vector<std::string> &v)
            {
                _contents = v; this->write();
            }

            virtual void display(std::ostream &);
    };

    /* generic directory object */
    template <class C>
    class base_dir_T : public fileobject_T
    {
        protected:
            bool _recurse;
            DIR *_dir;
            std::vector<C> _contents;

        public:
            typedef typename std::vector<C>::iterator iterator;
            typedef typename std::vector<C>::const_iterator const_iterator;
            typedef typename std::vector<C>::size_type size_type;

            base_dir_T(const char *n, bool r = false)
                : fileobject_T(n, FTYPE_DIR), _recurse(r), _dir(NULL)
            { this->open(); }

            base_dir_T(const std::string &n, bool r = false)
                : fileobject_T(n, FTYPE_DIR), _recurse(r), _dir(NULL)
            { this->open(); }

            virtual ~base_dir_T() { this->close(); }

            /* small subset of vector methods */
            iterator begin() { return _contents.begin(); }
            iterator end() { return _contents.end(); }
            size_type bufsize() const { return _contents.size(); }

            virtual void open();
            virtual void open(const char *n)
            {
                _path.assign(n);
                this->stat();
                this->open();
            }

            virtual void close();
            virtual void display(std::ostream &);
            virtual void read() = 0;
    };

    /* represents a fileobject_T container (aka a directory).
     * of course, directories are file objects themselves... */
    class dirobject_T : public base_dir_T<fileobject_T * >
    {
        public:

            dirobject_T(const char *n, bool r = false)
                : base_dir_T<fileobject_T * >(n, r) { this->read(); }
            dirobject_T(const std::string &n, bool r = false)
                : base_dir_T<fileobject_T * >(n, r) { this->read(); }
            virtual ~dirobject_T();
            virtual void read();
            virtual void display(std::ostream &);
    };

    /* acts as a DIR/dirent wrapper (only provides
     * the filename) instead of creating fileobject_T
     * objects for each child like dirobject_T */
    class dir_T : public base_dir_T<path_T>
    {
        public:
            dir_T(const char *n, bool r = false)
                : base_dir_T<path_T>(n, r) { this->read(); }
            dir_T(const std::string &n, bool r = false) 
                : base_dir_T<path_T>(n, r) { this->read(); }
            virtual ~dir_T() { }
            virtual void read();
    };
}

#endif

/* vim: set tw=80 sw=4 et : */
