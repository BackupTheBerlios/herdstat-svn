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
#include "metadatas.hh"
#include "action_handler.hh"

class action_pkg_handler_T : public action_herds_xml_handler_T
{
    public:
        action_pkg_handler_T() : action_herds_xml_handler_T(),
                                 elapsed(0),
                                 dev(optget("dev", bool)),
                                 status(not quiet and not debug) { }
        virtual ~action_pkg_handler_T() { }
	virtual int operator() (opts_type &);

    private:
        class package_list : public std::map<util::string, util::string>
        {
            public:
                package_list(const opts_type::value_type &n) : name(n) { }

            herds_xml_T::dev_type attr;
            opts_type::value_type name;
        };

        void search(package_list *);
        void display(package_list &);

        metadatas_T metadatas;
        util::progress_T  progress;
        util::timer_T::size_type elapsed;
        const bool dev, status;
};

#endif

/* vim: set tw=80 sw=4 et : */
