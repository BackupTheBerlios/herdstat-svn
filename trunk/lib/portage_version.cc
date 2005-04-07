/*
 * herdstat -- lib/portage_version.cc
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
#include <vector>
#include <iterator>
#include <cassert>

#include "string.hh"
#include "portage_version.hh"

/*
 * Display full version string.
 */

const std::string
portage::version_string_T::operator() () const
{
    /* chop -r0 if necessary */
    std::string::size_type pos = _verstr.rfind("-r0");
    if (pos != std::string::npos)
        return _verstr.substr(0, pos);

    return _verstr;
}

/*
 * Split full version string into components P, PV, PN, etc
 * and save each one in our internal map.
 */

void
portage::version_string_T::split_version()
{
    std::string::size_type pos;
    std::vector<std::string> parts, comps;
    std::vector<std::string>::iterator i;

    assert(not _verstr.empty());

    /* append -r0 if necessary */
    if ((pos = _verstr.rfind("-r")) == std::string::npos)
        _verstr.append("-r0");

    parts = util::split(_verstr, '-');

    /* If parts > 3, ${PN} contains a '-' */
    if (parts.size() > 3)
    {
        /* reconstruct ${PN} */
        std::string PN = parts.front();
        parts.erase(parts.begin());

        while (parts.size() >= 3)
        {
            PN += "-" + parts.front();
            parts.erase(parts.begin());
        }

        comps.push_back(PN);
    }

    std::copy(parts.begin(), parts.end(), std::back_inserter(comps));
    assert(comps.size() == 3);

    /* fill our map with the components */
    _v["PN"] = comps[0];
    _v["PV"] = comps[1];
    _v["PR"] = comps[2];
    _v["P"]   = _v["PN"] + "-" + _v["PV"];
    _v["PVR"] = _v["PV"] + "-" + _v["PR"];
    _v["PF"]  = _v["PN"] + "-" + _v["PVR"];

    _cmpstr = _v["PVR"];
}

/* vim: set tw=80 sw=4 et : */
