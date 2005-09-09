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

#include <ostream>
#include <utility>
#include <cassert>

#include <herdstat/exceptions.hh>
#include <herdstat/util/misc.hh>
#include <herdstat/util/vars.hh>

namespace util {
/****************************************************************************/
vars_T::vars_T() : _depth(0)
{
}
/****************************************************************************/
vars_T::vars_T(const std::string &path)
    : base_file_T(path), _depth(0)
{
    this->read();
}
/****************************************************************************/
void
vars_T::dump(std::ostream &stream) const
{
    const_iterator i;
    for (i = this->begin() ; i != this->end() ; ++i)
        stream << i->first << "=" << i->second << std::endl;
}
/****************************************************************************/
void
vars_T::read(const std::string &path)
{
    this->stat().assign(path);
    this->read();
}
/****************************************************************************/
void
vars_T::set_defaults()
{
    char *result = std::getenv("HOME");
    if (result)
        this->insert(std::make_pair("HOME", result));
    else
    {
        std::string home("/home/");
        home += current_user();
        if (is_dir(home))
            this->insert(std::make_pair("HOME", home));
    }

    /* for derivatives to define their own defaults */
    this->do_set_defaults();
}
/****************************************************************************
 * Read from our stream, saving any VARIABLE=["']value['"]
 * statements in our map.  Lines beginning with a '#'
 * are considered to be comments.  Should work with shell
 * scripts or VARIABLE=value-type configuration files.
 ****************************************************************************/
void
vars_T::read()
{
    if (not this->is_open())
        this->open();

    std::string s;
    std::string::size_type pos;

    while (std::getline(*(this->stream), s))
    {
        std::string line(s);
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
 
            (void)this->erase(key);
            this->insert(std::make_pair(key, val));
        }
    }

    this->set_defaults();

    /* loop through our map performing variable substitutions */
    iterator e = this->end();
    for (iterator i = this->begin() ; i != e ; ++i)
        this->subst(i->second);
}
/****************************************************************************
 * Search the given variable value for any variable occurrences,
 * recursively calling ourselves each time we find another occurrence.
 ****************************************************************************/
void
vars_T::subst(std::string &value)
{

    std::vector<std::string> vars;
    std::vector<std::string>::iterator v, e;
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
        if (this->_depth < 20)
            vars.push_back(value.substr(begin + 2, end - (begin + 2)));
        
        lpos = ++end;
    }

    /* for each variable we found */
    e = vars.end();
    for (v = vars.begin() ; v != e ; ++v)
    {
        std::string subst;
        std::string var("${"+(*v)+"}");

        std::string::size_type pos = value.find(var);
        if (pos == std::string::npos)
            continue;

        /* is that variable defined? */
        iterator x = this->find(*v);
        if (x != this->end())
            subst = x->second;

        /* don't bother trying to perform substitution if they're equal */
        if (subst == value)
            continue;

        if (subst.find("${") != std::string::npos)
        {
            ++(this->_depth);
            this->subst(subst);
            --(this->_depth);
        }

        if (not subst.empty())
            value.replace(pos, var.length(), subst, 0, subst.length());
    }
}
/****************************************************************************/
} // namespace util

/* vim: set tw=80 sw=4 et : */
