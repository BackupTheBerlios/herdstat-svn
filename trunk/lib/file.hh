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

#include <fstream>
#include <ostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

namespace util
{
    enum type_T { dir, file };

    /* generic file object */
    class fileobject_T
    {
        protected:
            std::string _name;   /* file object's name */
            struct stat _sbuf;   /* stat structure */
            type_T _type;

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

            fileobject_T(const char *, type_T);
            fileobject_T(const std::string &, type_T);
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

            std::string &name() { return _name; }
            std::string basename() const { return util::basename(_name); }
            std::string dirname() const { return util::dirname(_name); }
            type_T type() const { return _type; }

            virtual void display(std::ostream &) { }
            virtual void open() = 0;
            virtual void open(const char *) = 0;
            virtual void read() = 0;
    };

    /* represents a regular file */
    template <class C>
    class file_T : public fileobject_T
    {
        protected:
            C *stream;

        public:
            file_T(const std::string &n)
                : fileobject_T(n, file), stream(NULL) { }
            file_T(const char *n)
                : fileobject_T(n, file), stream(NULL) { }
            file_T(const std::string &n, C *s)
                : fileobject_T(n, file), stream(s) { }
            file_T(const char *n, C *s)
                : fileobject_T(n, file), stream(s) { }
            virtual ~file_T() { if (stream) delete stream; }

            virtual void open();
            virtual void open(const char *);
            virtual void read();
    };

    typedef file_T<std::ofstream> ofile_T;
    typedef file_T<std::ifstream> ifile_T;

    /* represents a fileobject_T container (aka a directory).
     * of course, directories are file objects themselves... */
    class dir_T : public fileobject_T
    {
        protected:
            std::vector<fileobject_T * > _contents;

        public:
            typedef std::vector<fileobject_T * >::iterator iterator;
            typedef std::vector<fileobject_T * >::const_iterator const_iterator;
            typedef std::vector<fileobject_T * >::size_type size_type;

            dir_T(const char *n, bool recurse = true)
                : fileobject_T(n, dir) { read(recurse); }
            dir_T(const std::string &n, bool recurse = true)
                : fileobject_T(n, dir) { read(recurse); }
            virtual ~dir_T();

            virtual void open() { }
            virtual void open(const char *) { }
            virtual void read() { return false; }
            virtual void read(bool recurse);
            virtual void display(std::ostream &);

            /* small subset of vector methods */
            iterator begin() { return _contents.begin(); }
            iterator end() { return _contents.end(); }
            size_type size() const { return _contents.size(); }
    };

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
}

#endif

/* vim: set tw=80 sw=4 et : */
