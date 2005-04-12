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

#include "util_exceptions.hh"

namespace portage
{
    class bad_version_suffix_E : public util::msg_base_E
    {
        public:
            bad_version_suffix_E() { }
            bad_version_suffix_E(const char *msg) : util::msg_base_E(msg) { }
            bad_version_suffix_E(const std::string &msg)
                : util::msg_base_E(msg) { }
            virtual const char *what() const throw()
            {
                std::string s("Invalid version suffix: ");
                s += str;
                return s.c_str();
            }
    };

    class ambiguous_pkg_E : public util::msg_base_E
    {
        private:
            const std::vector<std::string> _v;

        public:
            ambiguous_pkg_E() { }
            ambiguous_pkg_E(const std::vector<std::string> &v) : _v(v) { }
            virtual const char *what() const throw()
            {
                if (not this->_v.empty())
                {
                    std::string::size_type pos = this->_v.front().find('/');
                    if (pos != std::string::npos)
                    {
                        std::cerr << this->_v.front().substr(pos)
                            << " is ambiguous.  Possibles matches are: "
                            << std::endl;
                    }
                }

                std::vector<std::string>::const_iterator i;
                for (i = this->_v.begin() ; i != this->_v.end() ; ++i)
                    std::cerr << "\033[0;32m" << *i << "\033[0;00m" << std::endl;

                return "";
            }
    };

    class nonexistent_pkg_E : public util::msg_base_E
    {
        public:
            nonexistent_pkg_E() { }
            nonexistent_pkg_E(const char *msg) : util::msg_base_E(msg) { }
            nonexistent_pkg_E(const std::string &msg) : util::msg_base_E(msg) { }
            virtual const char *what() const throw()
            {
                return (std::string(str) + " doesn't seem to exist.").c_str();
            }
    };
}

#endif

/* vim: set tw=80 sw=4 et : */
