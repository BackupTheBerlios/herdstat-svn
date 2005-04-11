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
#include <cstdlib>
#include <climits>
#include <cassert>

#ifdef HAVE_STDINT_H
/* It looks like glibc's stdint.h wraps the UINTMAX_MAX define
 * in a #if !defined __cplusplus || defined __STDC_LIMIT_MACROS,
 * so enable it, as we need it for strtoumax().  */
# ifndef __STDC_LIMIT_MACROS
#  define __STDC_LIMIT_MACROS
# endif /* __STDC_LIMIT_MACROS */
# include <stdint.h>
/* don't use strtoumax if UINTMAX_MAX is still unavailable */
# ifndef UINTMAX_MAX
#  undef HAVE_STRTOUMAX
# endif /* UINTMAX_MAX */
#endif /* HAVE_STDINT_H */

#ifdef HAVE_INTTYPES_H
# include <inttypes.h>
#endif /* HAVE_INTTYPES_H */

#include "string.hh"
#include "portage_exceptions.hh"
#include "portage_version.hh"

std::vector<std::string> portage::version_suffix_T::_suffixes;

void
portage::version_suffix_T::init(const std::string &s)
{
    /* valid suffixes (in order) */
    if (_suffixes.empty())
    {
        _suffixes.push_back("alpha");
        _suffixes.push_back("beta");
        _suffixes.push_back("pre");
        _suffixes.push_back("rc");
        _suffixes.push_back("p");
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
        if (std::find(_suffixes.begin(), _suffixes.end(),
            _suffix) == _suffixes.end())
            throw portage::bad_version_suffix_E(_suffix);
    }
}

bool
portage::version_suffix_T::operator< (version_suffix_T &that)
{
    std::vector<std::string>::iterator ti, si;

    ti = std::find(_suffixes.begin(), _suffixes.end(), this->suffix());
    si = std::find(_suffixes.begin(), _suffixes.end(), that.suffix());

    /* both have a suffix */
    if ((ti != _suffixes.end()) and (si != _suffixes.end()))
    {
        /* same suffix, so compare suffix version */
        if (ti == si)
        {
            if (not this->version().empty() and not that.version().empty())
            {
#ifdef HAVE_STRTOUL
                unsigned long thisver =
                    std::strtoul(this->version().c_str(), NULL, 10);
                unsigned long thatver =
                    std::strtoul(that.version().c_str(), NULL, 10);

                /* if all else fails, do a string comparison */
                if ((thisver == ULONG_MAX) or (thatver == ULONG_MAX))
                    return this->version() < that.version();
                else
                    return thisver < thatver;
#else
                return this->version() < that.version();
#endif /* HAVE_STRTOUL */
            }
            else if (this->version().empty() and that.version().empty())
                return true;
            else
                return ( that.version().empty() ? false : true );
        }

        return ti < si;
    }

    /* that has no suffix */
    else if (ti != _suffixes.end())
        /* only the 'p' suffix is > than no suffix */
        return (*ti == "p" ? false : true);
    
    /* this has no suffix */
    else if (si != _suffixes.end())
        /* only the 'p' suffix is > than no suffix */
        return (*si == "p" ? true : false);

    return false;
}

bool
portage::version_suffix_T::operator== (version_suffix_T &that)
{
    std::vector<std::string>::iterator ti, si;

    ti = std::find(_suffixes.begin(), _suffixes.end(), this->suffix());
    si = std::find(_suffixes.begin(), _suffixes.end(), that.suffix());

    /* both have a suffix */
    if ((ti != _suffixes.end()) and (si != _suffixes.end()))
    {
        /* same suffix, so compare suffix version */
        if (ti == si)
        {
            if (not this->version().empty() and not that.version().empty())
            {
#ifdef HAVE_STRTOUL
                unsigned long thisver =
                    std::strtoul(this->version().c_str(), NULL, 10);
                unsigned long thatver =
                    std::strtoul(that.version().c_str(), NULL, 10);

                /* if all else fails, do a string comparison */
                if ((thisver == ULONG_MAX) or (thatver == ULONG_MAX))
                    return this->version() == that.version();
                else
                    return thisver == thatver;
#else
                return this->version() == that.version();
#endif /* HAVE_STRTOUL */
            }
            else if (this->version().empty() and that.version().empty())
                return true;
            else
                return ( that.version().empty() ? false : true );
        }

        return ti == si;
    }
    else if ((ti != _suffixes.end()) or (si != _suffixes.end()))
        return false;

    return true;
}

void
portage::version_string_T::init()
{
    this->split();
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
//    std::cout << "-----------------------------------------" << std::endl;
//    std::cout << "Comparing this (" << this->_v["nosuffix"] + this->_suffix.suffix() + "-" + this->_v["PR"]
//        << ") to that (" << that["nosuffix"] + that._suffix.suffix() + "-" + that["PR"] << ")."
//        << std::endl;


//    std::cout << (this->_v["nosuffix"] < that["nosuffix"]) << std::endl;
//    std::cout << "this->version = " << this->_v["nosuffix"] << std::endl;
//    std::cout << "that->version = " << that["nosuffix"] << std::endl;

//    std::string ver(this->_v["nosuffix"]);
//    if (ver.find('.') != std::string::npos)
//    {
//        std::string::size_type lpos = 0;

//        while (true)
//        {
//            std::string::size_type pos = ver.find('.', lpos);
//            if (pos == std::string::npos)
//                break;

//            if (ver.substr(lpos, pos - lpos).length() == 1)
//            {
//                this->_v["nosuffix"].insert(pos - 1, "0", 0, 1);
//                _pos.push_back(pos - 2);
//            }

//            lpos == ++pos;
//    }

    if (this->_v["nosuffix"] < that["nosuffix"])
        return true;
    else if (this->_v["nosuffix"] == that["nosuffix"])
    {
//            std::cout << (this->_suffix < that._suffix) << std::endl;
//            std::cout << "this->suffix  = " << this->_suffix.suffix() << std::endl;
//            std::cout << "that->suffix  = " << that._suffix.suffix() << std::endl;

        if (this->_suffix < that._suffix)
            return true;
        else if (this->_suffix == that._suffix)
        {
//            std::cout << (this->_v["PR"] <= that["PR"]) << std::endl;
//            std::cout << "this->rev     = " << this->_v["PR"] << std::endl;
//            std::cout << "that->rev     = " << that["PR"] << std::endl;

#ifdef HAVE_STRTOUMAX
            uintmax_t thisrev = strtoumax(this->_v["PR"].substr(1).c_str(), NULL, 10);
            uintmax_t thatrev = strtoumax(that["PR"].substr(1).c_str(), NULL, 10);

            bool success = false;
            switch (thisrev)
            {
                case 0:
                    if (this->_v["PR"] == "r0")
                        success = true;
                    break;
                case INTMAX_MIN:
                case INTMAX_MAX:
                case UINTMAX_MAX:
                    break;
                default:
                    success = true;
            }

            if (success)
            {
                switch (thatrev)
                {
                    case 0:
                        if (that["PR"] == "r0")
                            return thisrev <= thatrev;
                        else
                            return this->_v["PR"] <= that["PR"];
                    case INTMAX_MIN:
                    case INTMAX_MAX:
                    case UINTMAX_MAX:
                        return this->_v["PR"] <= that["PR"];
                    default:
                        return thisrev <= thatrev;
                }
            }
#endif /* HAVE_STRTOUMAX */
            return this->_v["PR"] <= that["PR"];
        }
    }

    return false;
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
portage::version_string_T::split()
{
    std::string::size_type pos;
    std::vector<std::string> parts, comps;
    std::vector<std::string>::iterator i;

    assert(not _verstr.empty());

    /* append -r0 if necessary */
    if ((pos = _verstr.rfind("-r")) == std::string::npos)
        _verstr.append("-r0");

    parts = _verstr.split('-');

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
    else
        _v["nosuffix"] = _v["PV"];
}

/* vim: set tw=80 sw=4 et : */
