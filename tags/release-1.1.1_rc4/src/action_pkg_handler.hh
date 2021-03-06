/*
 * herdstat -- src/action_pkg_handler.hh
 * $Id$
 * Copyright (c) 2005 Aaron Walker <ka0ttic at gentoo.org>
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

#ifndef HAVE_ACTION_PKG_HANDLER_HH
#define HAVE_ACTION_PKG_HANDLER_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <map>
#include "herds_xml.hh"
#include "metadata_xml.hh"
#include "metacache.hh"
#include "pkgquery.hh"
#include "querycache.hh"
#include "action_handler.hh"

class action_pkg_handler_T : public action_herds_xml_handler_T
{
    public:
        action_pkg_handler_T() : action_herds_xml_handler_T(),
                                 metacache(portdir),
                                 optsize(0), elapsed(0),
                                 dev(optget("dev", bool)),
                                 meta(optget("meta", bool)),
                                 status(not quiet and not debug),
                                 cache_is_valid(optget("metacache", bool) and
                                          (metacache.valid())),
                                 at_least_one_not_cached(false) { }

        virtual ~action_pkg_handler_T() { }
	virtual int operator() (opts_type &);

    private:
        void search(const std::vector<util::string> &, pkgQuery_T &);
        void search(const opts_type &);
        void display();
        void display(pkgQuery_T *);
        void error(const util::string &) const;
        void cleanup();
        bool metadata_matches(const metadata_T &, const util::string &);

        std::map<opts_type::value_type, pkgQuery_T * > matches;
        opts_type not_found, packages;
        metacache_T metacache;
        querycache_T querycache;
        opts_type::size_type optsize;
        util::timer_T::size_type elapsed;
        const bool dev, meta, status, cache_is_valid;
        bool at_least_one_not_cached;
        util::regex_T with;
};

#endif

/* vim: set tw=80 sw=4 et : */
