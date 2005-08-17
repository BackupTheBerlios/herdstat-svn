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
    /** Is the given path a directory?
     * @param p Path.
     * @returns A boolean value.
     */
    bool is_dir(const std::string &p);

    /** Is the file associated with the given stat structure a directory?
     * @param s Reference to a struct stat.
     * @returns A boolean value.
     */
    bool is_dir(const ::stat &s);

    /** Is the given path a regular file?
     * @param p Path.
     * @returns A boolean value.
     */
    bool is_file(const std::string &p);

    /** Is the file associated with the given stat structure a regular file?
     * @param s Reference to a struct stat.
     * @returns A boolean value.
     */
    bool is_file(const ::stat &s);

    /** Return the basename of the given path.
     * @param p Path.
     * @returns A pointer of type char.
     */
    const char *basename(const std::string &p);
    
    /** Return the directory name the given path is located in.
     * @param p Path.
     * @returns A pointer of type char.
     */
    const char *dirname(const std::string &p);

    /** Chop file extension from the given path.
     * @param p Path.
     * @param depth Number of period-delimited extensions to chop.
     * @returns A pointer of type char.
     */
    const char *chop_fileext(const std::string &p, unsigned short depth = 1);

    /** Copy file 'from' to file 'to'.
     * @param from Source location.
     * @param to   Destination location.
     */
    void copy_file(const std::string &to, const std::string &from);

    /** Move file 'from' to file 'to'.
     * @param from Source location.
     * @param to   Destination location.
     */
    void move_file(const std::string &to, const std::string &from);

    /**
     * A wrapper for struct stat and the stat() system call.
     */

    enum __FileType { };

    class stat : public ::stat
    {
        public:
            typedef dev_t   device_type;
            typedef ino_t   inode_type;
            typedef mode_t  mode_type;
            typedef uid_t   uid_type;
            typedef gid_t   gid_type;
            typedef off_t   size_type;
            typedef time_t  time_type;
            typedef __FileType file_type;

            /// Default constructor.
            stat();

            /** Constructor.
             * @param p Path.
             * @param opened Has the file associated with this been opened?
             */
            stat(const std::string &p, bool opened = false);

            ~stat();

            device_type device()   const { return this->st_dev; }
            inode_type  inode()    const { return this->st_ino; }
            mode_type   mode()     const { return this->st_mode; }
            uid_type    uid()      const { return this->st_uid; }
            gid_type    gid()      const { return this->st_gid; }
            size_type   size()     const { return this->st_size; }
            time_type   atime()    const { return this->st_atime; }
            time_type   mtime()    const { return this->st_mtime; }
            time_type   ctime()    const { return this->st_ctime; }
            std::string &path()    const { return this->_path; }
            file_type    type()    const { return this->_type; }

            /// Assign a new path and stat it.
            void assign(const std::string &p, bool opened = false);

            /** Does the file associated with this exist?
             * @returns A boolean value.
             */
            bool exists() const { return this->_exists; }

            /** stat() wrapper.
             * @returns A boolean value (exists()).
             */
            bool operator() ();

            static file_type regular;
            static file_type directory;
            static file_type character;
            static file_type block;
            static file_type fifo;
            static file_type link;
            static file_type socket;

        protected:
            std::string _path;
            file_type _type;
            bool _exists, _opened;
    };

    /**
     * Base file object.
     */

    class base_fileobject
    {
        public:
            /// Default constructor.
            base_fileobject();

            /** Constructor.
             * @param path Path.
             */
            base_fileobject(const std::string &path);

            virtual ~base_fileobject();

            /** Has this file object been opened?
             * @returns A boolean value.
             */
            bool is_open() const { return this->_opened; }

            /** Dump our internal contents to disk.
             * @param s Output stream.
             */
            virtual void dump(std::ostream &s) const { }

            /// Open file object.
            virtual void open()     = 0;

            /// Close file object.
            virtual void close()    = 0;

            /// Read file object, filling internal container.
            virtual void read()     = 0;
            
        protected:
            /// path to file object.
            std::string  _path;
            /// stat object associated with this file object.
            stat  _stat;
            /// whether this file has been opened.
            bool  _opened;
    };

    /**
     * Base type for representing regular files.
     */

    class base_file : public base_fileobject
    {
        public:
            /// Default constructor.
            base_file();

            /** Constructor.  Opens file.
             * @param path Path to file.
             * @param mode Open mode (defaults to DEFAULT_MODE).
             */
            base_file(const std::string &path,
                      std::ios_base::openmode mode = DEFAULT_MODE);

            /// Destructor.  Closes file if opened.
            virtual ~base_file();

            /// Open file with default open mode.
            virtual void open() { this->open(this->_path.c_str(), DEFAULT_MODE); }

            /** Open specified path with specified open mode.
             * @param path Path to file.
             * @param mode Open mode (defaults to DEFAULT_MODE).
             */
            virtual void open(const char *path,
                std::ios_base::openmode mode = DEFAULT_MODE);

            /** Open with specified open mode.
             * @param mode Open mode.
             */
            virtual void open(std::ios_base::openmode mode)
            { this->open(this->_path.c_str(), mode); }

            /// Close file.
            virtual void close();

        protected:
            /// Stream associated with this file object.
            std::fstream *stream;
    };

    /**
     * Represents a regular file using a vector of strings
     * for storing file contents.
     */

    class file : public base_file
    {
        public:
            typedef std::string value_type;
            typedef std::vector<value_type>::iterator iterator;
            typedef std::vector<value_type>::const_iterator const_iterator;
            typedef std::vector<value_type>::reverse_iterator reverse_iterator;
            typedef std::vector<value_type>::const_reverse_iterator
                                             const_reverse_iterator;
            typedef std::vector<value_type>::size_type size_type;

            /** Constructor.  Opens and reads file.
             * @param path Path to file.
             * @param mode Open mode (defaults to DEFAULT_MODE).
             */
            file(const std::string &path,
                 std::ios_base::openmode mode = DEFAULT_MODE);

            virtual ~file() { }

            /** File size.
             * @returns An unsigned integer value.
             */
            stat::size_type size() const { return _stat.size(); }

            /** Internal buffer size (number of lines in file).
             * @returns An unsigned integer value.
             */
            size_type bufsize() const
            { return _contents.size(); }

            /** Determine if two files are equal.
             * @param f file object.
             * @returns A boolean value.
             */
            virtual bool operator== (const file &f) const;

            /** Determine if two files are not equal.
             * @param f file object.
             * @returns A boolean value.
             */
            virtual bool operator!= (const file &f) const
            { return not (*this == f); }

            /// Load file into internal container.
            virtual void read();

            /** Dump internal container to specified stream.
             * @param s Output stream.
             */
            virtual void dump(std::ostream &s) const;

            /// Dump internal container to disk.
            virtual void write();

            iterator begin() { return _contents.begin(); }
            const_iterator begin() const { return _contents.begin(); }
            iterator end() { return _contents.end(); }
            const_iterator end() const { return _contents.end(); }

            size_type size() const { return _contents.size(); }
            bool empty() const { return _contents.size() != 0; }

        private:
            std::vector<std::string> _contents;
    };

    /**
     * A directory using a vector of std::string's to represent
     * directory contents.
     */

    class dir  : public base_fileobject
    {
        public:
            typedef std::string value_type;
            typedef std::vector<value_type>::iterator iterator;
            typedef std::vector<value_type>::const_iterator const_iterator;
            typedef std::vector<value_type>::reverse_iterator reverse_iterator;
            typedef std::vector<value_type>::const_reverse_iterator
                                             const_reverse_iterator;
            typedef std::vector<value_type>::size_type size_type;

            /// Default constructor.
            dir();

            /** Constructor.  Opens and reads directory.
             * @param path Path.
             */
            dir(const std::string &path);

            /// Destructor. Closes directory if opened.
            virtual ~dir() { if (this->_opened) this->close(); }

            /// Open directory.
            virtual void open();

            /// Close directory.
            virtual void close();

            /// Read directory.
            virtual void read();

            /** Find element with the specified name.
             * @param p Path.
             * @returns An iterator to the element (or end() if not found).
             */
            virtual iterator find(const value_type &p);

            /** Find element that matches the specified regular expression.
             * @param r Reference to a util::regex object.
             * @returns An iterator to the element (or end() if not found).
             */
            virtual iterator find(const regex &r);

            /** Find element with the specified name.
             * @param p Path.
             * @returns A const_iterator to the element (or end() if not found).
             */
            virtual const_iterator find(const value_type &p) const;

            /** Find element that matches the specified regular expression.
             * @param r Reference to a util::regex object.
             * @returns A const_iterator to the element (or end() if not found).
             */
            virtual const_iterator find(const regex &r) const;

            iterator begin() { return _contents.begin(); }
            const_iterator begin() const { return _contents.begin(); }
            iterator end() { return _contents.end(); }
            const_iterator end() const { return _contents.end(); }

            size_type size() const { return _contents.size(); }
            bool empty() const { return _contents.size() != 0; }

        protected:
            /// Internal DIR pointer.
            DIR *_dirp;
            std::vector<std::string> _contents;
    };
}

#endif

/* vim: set tw=80 sw=4 et : */
