/*
 * herdstat -- src/io/action/print.cc
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

#include <herdstat/util/string.hh>
#include "io/action/print.hh"

using namespace herdstat;

const char * const
PrintActionHandler::id() const
{
    return "print";
}

const char * const
PrintActionHandler::desc() const
{
    return "Display the specified option's value.";
}

const char * const
PrintActionHandler::usage() const
{
    return "print <option>";
}

void
PrintActionHandler::operator()(const Query& query,
                               QueryResults * const results)
{
    for (Query::const_iterator q = query.begin() ; q != query.end() ; ++q)
    {
        try
        {
#define ADD_INT_IF_EQUAL(type, x) \
            if (q->second == #x) \
                results->add(util::stringify<type>(options.x()));
#define ADD_STR_IF_EQUAL(x) \
            if (q->second == #x) \
                results->add(options.x());

            ADD_INT_IF_EQUAL(bool, quiet)
            else ADD_INT_IF_EQUAL(bool, verbose)
            else ADD_INT_IF_EQUAL(bool, count)
            else ADD_INT_IF_EQUAL(bool, regex)
            else ADD_INT_IF_EQUAL(bool, eregex)
            else ADD_INT_IF_EQUAL(bool, overlay)
            else ADD_INT_IF_EQUAL(bool, qa)
            else ADD_INT_IF_EQUAL(bool, metacache)
            else ADD_INT_IF_EQUAL(bool, devaway)
            else ADD_INT_IF_EQUAL(bool, meta)
            else ADD_INT_IF_EQUAL(bool, dev)
            else ADD_INT_IF_EQUAL(int, querycache_max)
            else ADD_INT_IF_EQUAL(long, querycache_expire)
            else ADD_INT_IF_EQUAL(long, devaway_expire)
            else ADD_INT_IF_EQUAL(size_t, maxcol)
            else ADD_STR_IF_EQUAL(action)
            else ADD_STR_IF_EQUAL(cvsdir)
            else ADD_STR_IF_EQUAL(herdsxml)
            else ADD_STR_IF_EQUAL(devawayxml)
            else ADD_STR_IF_EQUAL(userinfoxml)
            else ADD_STR_IF_EQUAL(localstatedir)
            else ADD_STR_IF_EQUAL(metacache_expire)
            else ADD_STR_IF_EQUAL(locale)
            else ADD_STR_IF_EQUAL(iomethod)
            else ADD_STR_IF_EQUAL(portdir)
            else ADD_STR_IF_EQUAL(with_dev)
            else ADD_STR_IF_EQUAL(with_herd)
            else if (q->second == "version")
                results->add(PACKAGE"-"VERSION);
            else throw Exception("Unknown option '%s'.", q->second.c_str());

#undef ADD_INT_IF_EQUAL
#undef ADD_STR_IF_EQUAL
        }
        catch (const Exception& e)
        {
            results->add(e.what());
        }
    }
}

/* vim: set tw=80 sw=4 fdm=marker et : */
