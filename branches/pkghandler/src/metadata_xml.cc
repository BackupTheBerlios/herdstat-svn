/*
 * herdstat -- src/metadata_xml.cc
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

#include <algorithm>
#include "metadata_xml.hh"

/*
 * Does the specified developer exist in this metadata.xml?
 */

bool
metadata_xml_T::dev_exists(const herd_type::key_type &dev) const
{
    herd_type::key_type d(dev);
    if (dev.find('@') == herd_type::key_type::npos)
        d.append("@gentoo.org");
    return this->_handler->devs.find(d) != this->_handler->devs.end();
}

/*
 * Does the specified herd exist in this metadata.xml?
 */

bool
metadata_xml_T::herd_exists(const herds_type::value_type &herd) const
{
    return std::find(this->_handler->herds.begin(),
        this->_handler->herds.end(), herd) != this->_handler->herds.end();
}

/* vim: set tw=80 sw=4 et : */
