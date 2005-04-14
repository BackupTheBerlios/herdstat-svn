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

#include "string.hh"

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
    void copy_file(const std::string &, const std::string &);
    void move_file(const std::string &, const std::string &);

    enum type_T { FTYPE_FILE, FTYPE_DIR };

    /* Represents a path to a file object */
    class path_T : public util::string
    {
        public:
            explicit path_T() : util::string() { }
            path_T(const char *n) : util::string(n) { }
            path_T(const std::string &n) : util::string(n) { }
            path_T(const path_T &n) : util::string(n) { }

            const std::string basename() const { return util::basename(*this); }
            const std::string dirname() const { return util::dirname(*this); }

            /* same as util::string::split but with a diff default delim */
            virtual std::vector<std::string> split(const char delim = '/')
            { return util::string::split(delim); }
    };

    /* generic file object */
    class fileobject_T
    {
        protected:
            void stat()
            {
                this->_exists = ( ::stat(this->_path.c_str(),
                    &this->_sbuf) == 0 ? true : false );
            }

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
            fileobject_T(const path_T &path, type_T t) : _path(path), _type(t)
            { this->stat(); }
            virtual ~fileobject_T() { this->close(); }

            size_type size() const { return this->_sbuf.st_size; }
            time_type mtime() const { return this->_sbuf.st_mtime; }
            time_type atime() const { return this->_sbuf.st_atime; }
            time_type ctime() const { return this->_sbuf.st_ctime; }
            uid_type uid() const { return this->_sbuf.st_uid; }
            gid_type gid() const { return this->_sbuf.st_gid; }
            mode_type mode() const { return this->_sbuf.st_mode; }
            inode_type inode() const { return this->_sbuf.st_ino; }
            device_type device() const { return this->_sbuf.st_dev; }
            blksize_type blksize() const { return this->_sbuf.st_blksize; }
            blkcnt_type blkcnt() const { return this->_sbuf.st_blocks; }

            std::string &name() { return this->_path; }
            std::string basename() const { return this->_path.basename(); }
            std::string dirname() const { return this->_path.dirname(); }
            type_T type() const { return this->_type; }
            bool exists() const { return this->_exists; }

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
            file_T(const path_T &path)
                : fileobject_T(path, FTYPE_FILE), stream(NULL) { }
            file_T(const path_T &path, std::fstream *s)
                : fileobject_T(path, FTYPE_FILE), stream(NULL) { }
            virtual ~file_T() { }

            iterator begin() { return this->_contents.begin(); }
            iterator end() { return this->_contents.end(); }
            size_type bufsize() const { return this->_contents.size(); }
            void push_back(const std::string &s)
            { this->_contents.push_back(s); }

            virtual void open()
            { this->open(this->_path.c_str(), DEFAULT_MODE); }
            virtual void open(const char *n,
                std::ios_base::openmode mode = DEFAULT_MODE);
            virtual void open(std::ios_base::openmode mode)
            { this->open(this->_path.c_str(), mode); }

            virtual void close();

            virtual void read() { this->read(&(this->_contents)); }
            virtual void read(std::vector<std::string> *);

            virtual void write() { this->display(*(this->stream)); }
            virtual void write(const std::vector<std::string> &v)
            { this->_contents = v; this->write(); }

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

            base_dir_T(const path_T &path, bool r = false)
                : fileobject_T(path, FTYPE_DIR), _recurse(r), _dir(NULL)
            { this->open(); }

            virtual ~base_dir_T() { }

            /* small subset of vector methods */
            iterator begin() { return this->_contents.begin(); }
            iterator end() { return this->_contents.end(); }
            size_type bufsize() const { return this->_contents.size(); }

            virtual void open();
            virtual void open(const char *n)
            {
                this->_path.assign(n);
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
            dirobject_T(const path_T &path, bool r = false)
                : base_dir_T<fileobject_T * >(path, r) { this->read(); }
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
            dir_T(const path_T &path, bool r = false)
                : base_dir_T<path_T>(path, r) { this->read(); }
            virtual ~dir_T() { }
            virtual void read();
    };
}

#endif

/* vim: set tw=80 sw=4 et : */
