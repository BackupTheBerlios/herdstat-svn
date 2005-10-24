/*
 * herdstat -- action/handler.hh
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

#ifndef _HAVE_ACTION_HANDLER_HH
#define _HAVE_ACTION_HANDLER_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <herdstat/portage/herds_xml.hh>
#include <herdstat/portage/devaway_xml.hh>
#include <herdstat/portage/userinfo_xml.hh>

#include "query.hh"
#include "query_results.hh"

class ActionHandler
{
    public:
        virtual ~ActionHandler() { }

        virtual void operator()(const Query& query,
                                QueryResults * const results) = 0;
};

class XMLActionHandler : public ActionHandler
{
    public:
        virtual ~XMLActionHandler() { }

        virtual void operator()(const Query& query,
                                QueryResults * const results) = 0;

    protected:
        void fetch_and_parse();

        herdstat::portage::herds_xml herds_xml;
        herdstat::portage::devaway_xml devaway_xml;
        herdstat::portage::userinfo_xml userinfo_xml;
};

#endif /* _HAVE_ACTION_HANDLER_HH */

/* vim: set tw=80 sw=4 et : */
