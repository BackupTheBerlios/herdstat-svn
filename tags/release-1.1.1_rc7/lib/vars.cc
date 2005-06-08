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
#include "portage_misc.hh"

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

    std::string s;
    string_type::size_type pos;

    while (std::getline(*(this->stream), s))
    {
        string_type line(s);
        pos = line.find_first_not_of(" \t");
        if (pos != string_type::npos)
            line.erase(0, pos);

        if (line.length() < 1 or line[0] == '#')
            continue;

        pos = line.find('=');
        if (pos != string_type::npos)
        {
            string_type key = line.substr(0, pos);
            string_type val = line.substr(pos + 1);

            /* handle leading/trailing whitespace */
            if (string_type::npos != (pos = key.find_first_not_of(" \t")))
                key.erase(0, pos);
            if (string_type::npos != (pos = val.find_first_not_of(" \t")))
                val.erase(0, pos);
            if (string_type::npos != (pos = key.find_last_not_of(" \t")))
                key.erase(++pos);
            if (string_type::npos != (pos = val.find_last_not_of(" \t")))
                val.erase(++pos);
 
            /* handle quotes */
            if (string_type::npos != (pos = val.find_first_of("'\"")))
            {
                val.erase(pos, pos + 1);
                if (string_type::npos != (pos = val.find_last_of("'\"")))
                    val.erase(pos, pos + 1);
            }
 
            this->_keys[key] = val;
        }
    }

    /* are we an ebuild? */
    this->_ebuild = portage::is_ebuild(this->name());

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
        this->subst(i->second);
}

/*
 * Search the given variable value for any variable occurrences,
 * recursively calling ourselves each time we find another occurrence.
 */

void
util::vars_T::subst(string_type &value)
{

    std::vector<string_type> vars;
    std::vector<string_type>::iterator v;
    string_type::size_type lpos = 0;

    /* find variables that need substituting */
    while (true)
    {
        string_type::size_type begin = value.find("${", lpos);
        if (begin == string_type::npos)
            break;

        string_type::size_type end = value.find("}", begin);
        if (end == string_type::npos)
            break;

        /* save it */
        if (this->_depth < 20)
            vars.push_back(value.substr(begin + 2, end - (begin + 2)));
        
        lpos = ++end;
    }

    /* for each variable we found */
    for (v = vars.begin() ; v != vars.end() ; ++v)
    {
        string_type subst;
        string_type var("${"+(*v)+"}");

        string_type::size_type pos = value.find(var);
        if (pos == string_type::npos)
            continue;

        /* is that variable defined? */
        iterator x = this->find(*v);
        if (x != this->end())
            subst = x->second;

        if (subst.find("${") != string_type::npos)
        {
            ++(this->_depth);
            this->subst(subst);
            --(this->_depth);
        }

        if (not subst.empty())
            value.replace(pos, var.length(), subst, 0, subst.length());
    }
}

/* vim: set tw=80 sw=4 et : */
