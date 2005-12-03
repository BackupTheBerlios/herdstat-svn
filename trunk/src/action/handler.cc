/*
 * herdstat -- action/handler.cc
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

#include "common.hh"
#include "pkgcache.hh"
#include "action/handler.hh"

using namespace herdstat;

ActionHandler::ActionHandler()
    : options(GlobalOptions()),
      color(GlobalColorMap()), _err(false), _size(-1)
{
    regexp.set_cflags(options.eregex() ?
                util::Regex::icase|util::Regex::extended :
                util::Regex::icase);
}

bool
ActionHandler::allow_empty_query() const
{
    /* by default, action handlers require a non-empty query object */
    return false;
}

const char * const
ActionHandler::usage() const
{
    /* by default, just show the action name */
    return this->id();
}

void
ActionHandler::operator()(Query &query, QueryResults * const results)
{
    BacktraceContext c("ActionHandler::operator()");

    this->do_init(query, results);

    /* handle all target */
    if (query.all())
        this->do_all(query, results);
    /* handle regex */
    else if (options.regex())
        this->do_regex(query, results);

    /* fill results */
    this->do_results(query, results);

    /* if the handler didnt set the size, default to query.size() */
    if (this->_size == -1)
        this->_size = query.size();

    this->do_cleanup(results);
}

void
ActionHandler::do_all(Query& query, QueryResults * const results)
{
    results->add("This handler does not support the all target.");
    throw ActionException();
}

void
ActionHandler::do_cleanup(QueryResults * const results)
{
    /* show count, if requested */
    if (options.count() and not this->error())
        results->add(this->_size);

    this->_size = this->_err = 0;
}

PortageSearchActionHandler::PortageSearchActionHandler()
    : matches(), _find(NULL)
{
}

PortageSearchActionHandler::~PortageSearchActionHandler()
{
    if (_find)
        delete _find;
}

void
PortageSearchActionHandler::do_regex(Query& query,
                                     QueryResults * const results)
{
    BacktraceContext c("PortageSearchActionHandler::do_regex("+query.front().second+")");

    regexp.assign(query.front().second);

    try
    {
        matches = find()(regexp);
        find().clear_results();

        if (not options.overlay())
        {
            remove_overlay_packages();

            /* might be empty if the pkg only exists in an overlay */
            if (matches.empty())
                throw portage::NonExistentPkg(regexp);
        }
    }
    catch (const portage::NonExistentPkg& e)
    {
        results->add(e.what());
        throw ActionException();
    }
}

void
PortageSearchActionHandler::do_cleanup(QueryResults * const results)
{
    ActionHandler::do_cleanup(results);
    matches.clear();

    if (options.timer())
        options.outstream() << "Took " << find().elapsed()
            << "ms to perform search." << std::endl;
}

/* vim: set tw=80 sw=4 fdm=marker et : */
