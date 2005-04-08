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
#include "portage_exceptions.hh"
#include "portage_version.hh"

std::vector<std::string> portage::version_suffix_T::_suffices;

void
portage::version_suffix_T::init(const std::string &s)
{
    /* valid suffices (in order) */
    if (_suffices.empty())
    {
        _suffices.push_back("alpha");
        _suffices.push_back("beta");
        _suffices.push_back("pre");
        _suffices.push_back("rc");
        _suffices.push_back("p");
    }

    this->get_suffix(s);
}

void
portage::version_suffix_T::get_suffix(const std::string &s)
{
    std::string result(s);
    std::string::size_type pos = result.rfind("-r0");
    if (pos != std::string::npos)
        result = result.substr(0, pos);

    if ((pos = result.rfind('_')) != std::string::npos)
    {
        _suffix = result.substr(pos + 1);
        
        /* chop any trailing suffix version */
        pos = _suffix.find_first_of("0123456789");
        if (pos != std::string::npos)
        {
            _suffix_ver = _suffix.substr(pos);
            _suffix = _suffix.substr(0, pos);
        }

        /* valid suffix? */
        if (std::find(_suffices.begin(), _suffices.end(),
            _suffix) == _suffices.end())
            throw portage::bad_version_suffix_E(_suffix);
    }
}

bool
portage::version_suffix_T::operator< (version_suffix_T &that)
{
    std::vector<std::string>::iterator ti, si;

    ti = std::find(_suffices.begin(), _suffices.end(), this->suffix());
    si = std::find(_suffices.begin(), _suffices.end(), that.suffix());

    /* both have a suffix */
    if ((ti != _suffices.end()) and (si != _suffices.end()))
    {
        /* same suffix, so compare suffix version */
        if (ti == si)
        {
            if (not this->version().empty() and not that.version().empty())
                return this->version() < that.version();
            else
                return ( that.version().empty() ? false : true );
        }

        return ti < si;
    }

    /* that has no suffix */
    else if (ti != _suffices.end())
        /* only the 'p' suffix is > than no suffix */
        return (*ti == "p" ? false : true);
    
    /* this has no suffix */
    else if (si != _suffices.end())
        /* only the 'p' suffix is > than no suffix */
        return (*si == "p" ? true : false);

    return false;
}

bool
portage::version_suffix_T::operator== (version_suffix_T &that)
{
    std::vector<std::string>::iterator ti, si;

    ti = std::find(_suffices.begin(), _suffices.end(), this->suffix());
    si = std::find(_suffices.begin(), _suffices.end(), that.suffix());

    /* both have a suffix */
    if ((ti != _suffices.end()) and (si != _suffices.end()))
    {
        /* same suffix, so compare suffix version */
        if (ti == si)
        {
            if (not this->version().empty() and not that.version().empty())
                return this->version() == that.version();
            else
                return ( that.version().empty() ? false : true );
        }

        return ti == si;
    }

    return false;
}

void
portage::version_string_T::init()
{
    this->split_verstr();
    _suffix = _v["PVR"];
}

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

bool
portage::version_string_T::operator< (version_string_T &that)
{
    return ( (this->_v["nosuffix"] < that["nosuffix"]) and
             (this->_suffix < that._suffix) and
             (this->_v["PR"] < that["PR"]) );
}

bool
portage::version_string_T::operator== (version_string_T &that)
{
    return ( (this->_v["nosuffix"] == that["nosuffix"]) and
             (this->_suffix == that._suffix) and
             (this->_v["PR"] == that["PR"]) );
}

/*
 * Split full version string into components P, PV, PN, etc
 * and save each one in our internal map.
 */

void
portage::version_string_T::split_verstr()
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

    /* save PV minus suffix */
    if ((pos = _v["PV"].rfind('_')) != std::string::npos)
        _v["nosuffix"] = _v["PV"].substr(0, pos);
}

/* vim: set tw=80 sw=4 et : */
