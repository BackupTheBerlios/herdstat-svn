/*
 * herdstat -- xml/exceptions.hh
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

#ifndef _HAVE_XML_EXCEPTIONS_HH
#define _HAVE_XML_EXCEPTIONS_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <herdstat/exceptions.hh>

namespace xml {

    /**
     * XML Parser exception.
     */

    class ParserException : public Exception
    {
        public:
            ParserException() : _file(), _error() { }
            ParserException(const std::string &f, const std::string &e)
                : _file(f), _error(e) { }
            virtual ~ParserException() throw() { }

            virtual const std::string& file() const { return _file; }
            virtual const std::string& error() const { return _error; }

        private:
            std::string _file, _error;
    };

} // namespace xml

#endif /* _HAVE_XML_EXCEPTIONS_HH */

/* vim: set tw=80 sw=4 et : */
