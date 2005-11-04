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
PrintIOActionHandler::id() const
{
    return "print";
}

const char * const
PrintIOActionHandler::desc() const
{
    return "Display the specified option's value.";
}

const char * const
PrintIOActionHandler::usage() const
{
    return "print <option>";
}

void
PrintIOActionHandler::operator()(const Query& query,
                               QueryResults * const results)
{
    for (Query::const_iterator q = query.begin() ; q != query.end() ; ++q)
    {
        try
        {
#define ADD_IF_EQUAL(x) \
            if (q->second == #x) \
                results->add(options.x());

            ADD_IF_EQUAL(quiet)
            else ADD_IF_EQUAL(verbose)
            else ADD_IF_EQUAL(count)
            else ADD_IF_EQUAL(regex)
            else ADD_IF_EQUAL(eregex)
            else ADD_IF_EQUAL(overlay)
            else ADD_IF_EQUAL(qa)
            else ADD_IF_EQUAL(metacache)
            else ADD_IF_EQUAL(devaway)
            else ADD_IF_EQUAL(meta)
            else ADD_IF_EQUAL(dev)
            else ADD_IF_EQUAL(querycache_max)
            else ADD_IF_EQUAL(querycache_expire)
            else ADD_IF_EQUAL(devaway_expire)
            else ADD_IF_EQUAL(maxcol)
            else ADD_IF_EQUAL(action)
            else ADD_IF_EQUAL(cvsdir)
            else ADD_IF_EQUAL(herdsxml)
            else ADD_IF_EQUAL(devawayxml)
            else ADD_IF_EQUAL(userinfoxml)
            else ADD_IF_EQUAL(localstatedir)
            else ADD_IF_EQUAL(metacache_expire)
            else ADD_IF_EQUAL(locale)
            else ADD_IF_EQUAL(iomethod)
            else ADD_IF_EQUAL(portdir)
            else ADD_IF_EQUAL(with_dev)
            else ADD_IF_EQUAL(with_herd)
            else if (q->second == "version")
                results->add(PACKAGE"-"VERSION);
            else throw Exception("Unknown option '%s'.", q->second.c_str());

#undef ADD_IF_EQUAL
        }
        catch (const Exception& e)
        {
            results->add(e.what());
        }
    }
}

/* vim: set tw=80 sw=4 fdm=marker et : */
