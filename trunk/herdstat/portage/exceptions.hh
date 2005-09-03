/*
 * herdstat -- lib/portage_exceptions.hh
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

#ifndef HAVE_PORTAGE_EXCEPTIONS_HH
#define HAVE_PORTAGE_EXCEPTIONS_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <iostream>
#include <vector>

#include <herdstat/exceptions.hh>

namespace portage
{
    /**
     * Bad version suffix exception.
     */

    class BadVersionSuffix : public Exception
    {
        public:
            BadVersionSuffix() { }
            BadVersionSuffix(const char *msg) : Exception(msg) { }
            BadVersionSuffix(const std::string &msg) : Exception(msg) { }
            virtual ~BadVersionSuffix() throw() { }
            virtual const char *what() const throw()
            {
                std::string s("Invalid version suffix: ");
                s += this->message();
                return s.c_str();
            }
    };

    /**
     * Ambiguous package name exception.
     */

    class AmbiguousPkg : public Exception
    {
        protected:
            /// Ambiguous package name.
            std::string _name;

        public:
            AmbiguousPkg() { }
            AmbiguousPkg(const std::vector<std::string> &v) : packages(v) { }
            virtual ~AmbiguousPkg() throw() { }

            virtual const std::string name() const
            {
                std::string s;
                if (not this->packages.empty())
                {
                    std::string::size_type pos = this->packages.front().find('/');
                    if (pos == std::string::npos)
                        s = this->packages.front();
                    else
                        s = this->packages.front().substr(pos + 1);
                }
                return s;
            }

            /// Vector of possible matches.
            const std::vector<std::string> packages;
    };

    /**
     * Non-existent package exception.
     */

    class NonExistentPkg : public Exception
    {
        public:
            NonExistentPkg() { }
            NonExistentPkg(const char *msg) : Exception(msg) { }
            NonExistentPkg(const std::string &msg) : Exception(msg) { }
            virtual ~NonExistentPkg() throw() { }
            virtual const char *what() const throw()
            {
                return (std::string(this->message()) +
                        " doesn't seem to exist.").c_str();
            }
    };

    /**
     * Quality Assurance exception.
     */

    class QAException : public Exception
    {
        public:
            QAException() { }
            QAException(const char *msg) : Exception(msg) { }
            QAException(const std::string &msg) : Exception(msg) { }
            virtual ~QAException() throw() { }
    };

    /**
     * Quality Assurance (with strerror()) exception.
     */

    class QAErrnoException : public ErrnoException
    {
        public:
            QAErrnoException() { }
            QAErrnoException(const char *msg) : ErrnoException(msg) { }
            QAErrnoException(const std::string &msg) : ErrnoException(msg) { }
            virtual ~QAErrnoException() throw() { }
            virtual const char *what() const throw()
            {
                std::string s("QA Violation: ");
                s += ErrnoException::what();
                return s.c_str();
            }
    };
}

#endif

/* vim: set tw=80 sw=4 et : */
