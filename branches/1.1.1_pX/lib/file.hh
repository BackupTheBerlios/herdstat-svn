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
    inline bool
    is_dir(const std::string &p)
    {
        struct stat s;
        return ((stat(p.c_str(), &s) == 0) ? S_ISDIR(s.st_mode) : false);
    }

    /** Is the file associated with the given stat structure a directory?
     * @param s Reference to a struct stat.
     * @returns A boolean value.
     */
    inline bool is_dir(const struct stat &s) { return S_ISDIR(s.st_mode); }

    /** Is the given path a regular file?
     * @param p Path.
     * @returns A boolean value.
     */
    inline bool
    is_file(const std::string &p)
    {
        struct stat s;
        return ((stat(p.c_str(), &s) == 0) ? S_ISREG(s.st_mode) : false);
    }

    /** Is the file associated with the given stat structure a regular file?
     * @param s Reference to a struct stat.
     * @returns A boolean value.
     */
    inline bool is_file(const struct stat &s) { return S_ISREG(s.st_mode); }

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

    /// Denotes file type.
    enum ftype_T { REGULAR, DIRECTORY, CHARACTER, BLOCK, FIFO, LINK, SOCKET };

    /**
     * A wrapper for struct stat and the stat() system call.
     */

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

            /// Default constructor.
            stat_T() : _type(REGULAR), _exists(false), _opened(false) { }

            /** Constructor.
             * @param p Path.
             * @param opened Has the file associated with this been opened?
             */
            stat_T(const std::string &p, bool opened = false)
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
            std::string path()     const { return this->_path; }
            ftype_T     type()     const { return this->_type; }

            /// Assign a new path and stat it.
            void assign(const std::string &p, bool opened = false)
            {
                this->_opened = opened;
                this->_path.assign(p);
                (void)(*this)();
            }

            /** Does the file associated with this exist?
             * @returns A boolean value.
             */
            bool exists() const { return this->_exists; }

            /** stat() wrapper.
             * @returns A boolean value (exists()).
             */
            virtual bool operator() ();

        protected:
            std::string _path;
            ftype_T _type;
            bool _exists, _opened;
    };

    /**
     * Abstract file object.
     */

    class base_fileobject_T
    {
        public:
            /// Default constructor.
            base_fileobject_T() : _opened(false) { }

            /** Constructor.
             * @param path Path.
             */
            base_fileobject_T(const std::string &path)
                : _path(path), _stat(path), _opened(false) { }

            virtual ~base_fileobject_T() { }

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
            stat_T  _stat;
            /// whether this file has been opened.
            bool    _opened;
    };

    /**
     * Base type for representing regular files.
     */

    class base_file_T : public base_fileobject_T
    {
        public:
            typedef std::fstream stream_type;

            /// Default constructor.
            base_file_T() : stream(NULL) { }

            /** Constructor.  Opens file.
             * @param path Path to file.
             * @param mode Open mode (defaults to DEFAULT_MODE).
             */
            base_file_T(const std::string &path, std::ios_base::openmode mode = DEFAULT_MODE)
                : base_fileobject_T(path), stream(NULL)
            { this->open(this->_path.c_str(), mode); }

            /// Destructor.  Closes file if opened.
            virtual ~base_file_T() { if (this->_opened) this->close(); }

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
            stream_type *stream;
    };

    /**
     * Represents a regular file using a vector of std::strings
     * for storing file contents.
     */

    class file_T : public base_file_T,
                   public std::vector<std::string>
    {
        public:
            /** Constructor.  Opens and reads file.
             * @param path Path to file.
             * @param mode Open mode (defaults to DEFAULT_MODE).
             */
            file_T(const std::string &path, std::ios_base::openmode mode = DEFAULT_MODE)
                : base_file_T(path, mode)
            { this->read(); }

            virtual ~file_T() { }

            /** File size.
             * @returns An unsigned integer value.
             */
            stat_T::size_type size() const { return this->_stat.size(); }

            /** Internal buffer size (number of lines in file).
             * @returns An unsigned integer value.
             */
            size_type bufsize() const
            { return std::vector<std::string>::size(); }

            /** Determine if two files are equal.
             * @param f file_T object.
             * @returns A boolean value.
             */
            virtual bool operator== (const file_T &f) const;

            /** Determine if two files are not equal.
             * @param f file_T object.
             * @returns A boolean value.
             */
            virtual bool operator!= (const file_T &f) const
            { return not (*this == f); }

            /// Load file into internal container.
            virtual void read();

            /** Dump internal container to specified stream.
             * @param s Output stream.
             */
            virtual void dump(std::ostream &s) const;

            /// Dump internal container to disk.
            virtual void write()
            {
                this->dump(*(this->stream));
                this->clear();
            }
    };

    /**
     * A directory using a vector of std::string's to represent
     * directory contents.
     */

    class dir_T  : public base_fileobject_T,
                   public std::vector<std::string>
    {
        public:
            /// Default constructor.
            dir_T() : _dirp(NULL) { }

            /** Constructor.  Opens and reads directory.
             * @param path Path.
             */
            dir_T(const std::string &path)
                : base_fileobject_T(path), _dirp(NULL)
            { this->open(); this->read(); }

            /// Destructor. Closes directory if opened.
            virtual ~dir_T() { if (this->_opened) this->close(); }

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
             * @param r Reference to a util::regex_T object.
             * @returns An iterator to the element (or end() if not found).
             */
            virtual iterator find(const regex_T &r);

            /** Find element with the specified name.
             * @param p Path.
             * @returns A const_iterator to the element (or end() if not found).
             */
            virtual const_iterator find(const value_type &p) const;

            /** Find element that matches the specified regular expression.
             * @param r Reference to a util::regex_T object.
             * @returns A const_iterator to the element (or end() if not found).
             */
            virtual const_iterator find(const regex_T &r) const;

        protected:
            /// Internal DIR pointer.
            DIR *_dirp;
    };
}

#endif

/* vim: set tw=80 sw=4 et : */
