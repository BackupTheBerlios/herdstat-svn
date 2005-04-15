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

#include <iostream>
#include <cassert>

#include "vars.hh"
#include "util_exceptions.hh"
#include "portage_version.hh"
#include "util.hh"

void
util::vars_T::read(const util::path_T &path)
{
    this->_path.assign(path);
    this->read();
}

/*
 * Read from our stream, saving any VARIABLE=["']value['"]
 * statements in our map.  Lines beginning with a '#'
 * are considered to be comments.  Should work with shell
 * scripts or VARIABLE=value-type configuration files.
 */

void
util::vars_T::read()
{
    if (not this->stream or not this->stream->is_open())
        this->open();

    std::string line;
    std::string::size_type pos;

    while (std::getline(*(this->stream), line))
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
 
            this->_keys[key] = val;
        }
    }

    /* are we an ebuild? */
    this->_ebuild = ((this->name().length() > 7) and
            (this->name().substr(this->name().length() - 7) == ".ebuild"));

    /* if so, insert its variable components
     * (${P}, ${PN}, ${PV}, etc) into our map */
    if (this->_ebuild)
    {
        portage::version_string_T version(this->name());
        portage::version_string_T::iterator v;

        for (v = version.begin() ; v != version.end() ; ++v)
            this->_keys[v->first] = v->second;
    }

    /* loop through our map performing variable substitutions */
    for (iterator i = this->_keys.begin() ; i != this->_keys.end() ; ++i)
        this->subst(i->first, i->second);
}

/*
 * Search the given variable value for any variable occurrences,
 * recursively calling ourselves each time we find another occurrence.
 */

void
util::vars_T::subst(const std::string &key, std::string &value)
{
    if (value.find("${") != std::string::npos)
    {
        std::vector<std::string> vars;
        std::vector<std::string>::iterator v;
        std::string::size_type lpos = 0;

        /* find variables that need substituting */
        while (true)
        {
            std::string::size_type begin = value.find("${", lpos);
            if (begin == std::string::npos)
                break;

            std::string::size_type end = value.find("}", begin);
            if (end == std::string::npos)
                break;

            /* save it */
            if (value.substr(begin + 2, end - (begin + 2)) != key)
                vars.push_back(value.substr(begin + 2, end - (begin + 2)));
            lpos = ++end;
        }

        /* for each variable we found */
        for (v = vars.begin() ; v != vars.end() ; ++v)
        {
            std::string subst;
            std::string var("${"+(*v)+"}");

            std::string::size_type pos = value.find(var);
            if (pos == std::string::npos)
                continue;

//            if (this->_ebuild)
//                util::debug("Found variable '%s'", var.c_str());

            /* is that variable defined? */
            iterator x = this->find(*v);
            if (x != this->end())
            {
                subst = x->second;
//                if (this->_ebuild)
//                    util::debug("Found value '%s'", subst.c_str());
            }

            if (subst.find("${") != std::string::npos)
                this->subst(*v, subst);

            if (not subst.empty())
                value.replace(pos, var.length(), subst, 0, subst.length());
        }
    }
}

/* vim: set tw=80 sw=4 et : */
