/*
 * herdstat -- src/action_dev_handler.hh
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

#ifndef HAVE_ACTION_DEV_HANDLER_HH
#define HAVE_ACTION_DEV_HANDLER_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <herdstat/portage/userinfo_xml.hh>
#include "action_handler.hh"

class action_dev_handler_T : public action_herds_xml_handler_T
{
    public:
        action_dev_handler_T();
        virtual ~action_dev_handler_T();
        virtual int operator() (opts_type &);

    private:
        void display(const std::string &);

        herdstat::portage::userinfo_xml userinfo;
};

#endif

/* vim: set tw=80 sw=4 et : */
