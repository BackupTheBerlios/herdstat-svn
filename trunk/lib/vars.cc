/*
 * herdstat -- lib/vars.cc
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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <memory>

#include "vars.hh"
#include "util_exceptions.hh"

void
util::vars_T::read()
{
    std::auto_ptr<std::ifstream> f(new std::ifstream(_file.c_str()));
    if (not (*f))
        throw util::bad_fileobject_E(_file);

    this->read(*f);
}

void
util::vars_T::read(const char *path)
{
    _file.assign(path);

    std::auto_ptr<std::ifstream> f(new std::ifstream(path));
    if (not (*f))
        throw util::bad_fileobject_E(path);

    this->read(*f);
}

void
util::vars_T::read(const std::string &path)
{
    return this->read(path.c_str());
}

void
util::vars_T::read(std::ifstream &stream)
{
    std::string line;
    std::string::size_type pos;

    while (std::getline(stream, line))
    {
        pos = line.find_first_not_of(" \t");
        if (pos != std::string::npos)
            line.erase(0, pos);

        if (line.length() < 1 or line[0] == '#')
            continue;

        pos = line.find('=');
        if (pos != std::string::npos)
        {
            std::string key = line.substr(0, pos);
            std::string val = line.substr(pos + 1);

            /* handle leading/trailing whitespace */
            if (std::string::npos != (pos = key.find_first_not_of(" \t")))
                key.erase(0, pos);
            if (std::string::npos != (pos = val.find_first_not_of(" \t")))
                val.erase(0, pos);
            if (std::string::npos != (pos = key.find_last_not_of(" \t")))
                key.erase(++pos);
            if (std::string::npos != (pos = val.find_last_not_of(" \t")))
                val.erase(++pos);
 
            /* handle quotes */
            if (std::string::npos != (pos = val.find_first_of("'\"")))
            {
                val.erase(pos, pos + 1);
                if (std::string::npos != (pos = val.find_last_of("'\"")))
                    val.erase(pos, pos + 1);
            }
 
            _keys[key] = val;
        }
    }
}

/* vim: set tw=80 sw=4 et : */
