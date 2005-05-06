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

#include "string.hh"
#include "util_exceptions.hh"

namespace portage
{
    class bad_version_suffix_E : public util::msg_base_E
    {
        public:
            bad_version_suffix_E() { }
            bad_version_suffix_E(const char *msg) : util::msg_base_E(msg) { }
            bad_version_suffix_E(const util::string &msg)
                : util::msg_base_E(msg) { }
            virtual const char *what() const throw()
            {
                util::string s("Invalid version suffix: ");
                s += this->str;
                return s.c_str();
            }
    };

    class ambiguous_pkg_E : public util::base_E
    {
        protected:
            util::string _name;

        public:
            ambiguous_pkg_E() { }
            ambiguous_pkg_E(const std::vector<util::string> &v) : packages(v) { }
            virtual ~ambiguous_pkg_E() throw() { }

            virtual const util::string name() const
            {
                util::string s;
                if (not this->packages.empty())
                {
                    util::string::size_type pos = this->packages.front().find('/');
                    if (pos == util::string::npos)
                        s = this->packages.front();
                    else
                        s = this->packages.front().substr(pos + 1);
                }
                return s;
            }

            const std::vector<util::string> packages;
    };

    class nonexistent_pkg_E : public util::msg_base_E
    {
        public:
            nonexistent_pkg_E() { }
            nonexistent_pkg_E(const char *msg) : util::msg_base_E(msg) { }
            nonexistent_pkg_E(const util::string &msg) : util::msg_base_E(msg) { }
            virtual const char *what() const throw()
            {
                return (util::string(this->str) +
                        " doesn't seem to exist.").c_str();
            }
    };

    class qa_E : public util::base_E { };

    class qa_errno_E : public util::errno_E
    {
        public:
            qa_errno_E() { }
            qa_errno_E(const char *msg) : util::errno_E(msg) { }
            qa_errno_E(const util::string &msg) : util::errno_E(msg) { }
            virtual const char *what() const throw()
            {
                return util::sprintf("QA Violation: %s",
                    util::errno_E::what()).c_str();
            }
    };
}

#endif

/* vim: set tw=80 sw=4 et : */
