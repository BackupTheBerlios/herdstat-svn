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

#ifdef HAVE_STDINT_H
/* It looks like glibc's stdint.h wraps the UINTMAX_MAX define
 * in a #if !defined __cplusplus || defined __STDC_LIMIT_MACROS,
 * so enable it, as we need it to check the return value of strtoumax(). */
#ifndef __STDC_LIMIT_MACROS
# define __STDC_LIMIT_MACROS
#endif /* __STDC_LIMIT_MACROS */
# include <stdint.h>
/* don't use strtoumax if UINTMAX_MAX is still unavailable */
# ifndef UINTMAX_MAX
#  undef HAVE_STRTOUMAX
# endif /* UINTMAX_MAX */
#endif /* HAVE_STDINT_H */

#ifdef HAVE_INTTYPES_H
# include <inttypes.h>
#endif /* HAVE_INTTYPES_H */

#include <iostream>
#include <vector>
#include <iterator>
#include <memory>
#include <cstdlib>
#include <climits>
#include <cassert>

#include "string.hh"
#include "portage_misc.hh"
#include "portage_exceptions.hh"
#include "portage_version.hh"

std::vector<std::string> portage::version_suffix_T::_suffixes;

/*
 * strtoumax wrapper
 */

uintmax_t
strtouint(const std::string &str)
{
#ifdef HAVE_STRTOUMAX
    uintmax_t i = strtoumax(str.c_str(), NULL, 10);

    switch (i)
    {
	case 0:
	    if (str == "0")
		return 0;
	    break;
	case INTMAX_MIN:
	case INTMAX_MAX:
	case UINTMAX_MAX:
	    break;
	default:
	    return i;
    }
#endif /* HAVE_STRTOUMAX */
    return std::atoi(str.c_str());
}

/*
 * strtoul wrapper
 */

unsigned long
strtoul(const std::string &str)
{
    unsigned long result = 0;

    result = std::strtoul(str.c_str(), NULL, 10);
    if (result == ULONG_MAX)
        result = 0;
    
    /* zero's only valid when str == "0" */
    if ((result == 0) and (str != "0"))
        result = std::atol(str.c_str());

    return result;
}

/********************
 * version_suffix_T *
 ********************/

void
portage::version_suffix_T::init(const std::string &s)
{
    /* valid suffixes (in order) */
    if (this->_suffixes.empty())
    {
        this->_suffixes.push_back("alpha");
        this->_suffixes.push_back("beta");
        this->_suffixes.push_back("pre");
        this->_suffixes.push_back("rc");
        this->_suffixes.push_back("p");
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
        this->_suffix = result.substr(pos + 1);
        
        /* chop any trailing suffix version */
        pos = this->_suffix.find_first_of("0123456789");
        if (pos != std::string::npos)
        {
            this->_suffix_ver = this->_suffix.substr(pos);
            this->_suffix = this->_suffix.substr(0, pos);
        }

        /* valid suffix? */
        if (std::find(this->_suffixes.begin(), this->_suffixes.end(),
            this->_suffix) == this->_suffixes.end())
            throw portage::bad_version_suffix_E(this->_suffix);
    }
}

bool
portage::version_suffix_T::operator< (version_suffix_T &that)
{
    std::vector<std::string>::iterator ti, si;

    ti = std::find(this->_suffixes.begin(), this->_suffixes.end(),
        this->suffix());
    si = std::find(this->_suffixes.begin(), this->_suffixes.end(),
        that.suffix());

    /* both have a suffix */
    if ((ti != this->_suffixes.end()) and (si != this->_suffixes.end()))
    {
        /* same suffix, so compare suffix version */
        if (ti == si)
        {
            if (not this->version().empty() and not that.version().empty())
                return ( strtoul(this->version()) < strtoul(that.version()) );
            else if (this->version().empty() and that.version().empty())
                return true;
            else
                return ( that.version().empty() ? false : true );
        }

        return ti < si;
    }

    /* that has no suffix */
    else if (ti != this->_suffixes.end())
        /* only the 'p' suffix is > than no suffix */
        return (*ti == "p" ? false : true);
    
    /* this has no suffix */
    else if (si != this->_suffixes.end())
        /* only the 'p' suffix is > than no suffix */
        return (*si == "p" ? true : false);

    return false;
}

bool
portage::version_suffix_T::operator== (version_suffix_T &that)
{
    std::vector<std::string>::iterator ti, si;

    ti = std::find(this->_suffixes.begin(), this->_suffixes.end(),
        this->suffix());
    si = std::find(this->_suffixes.begin(), this->_suffixes.end(),
        that.suffix());

    /* both have a suffix */
    if ((ti != this->_suffixes.end()) and (si != this->_suffixes.end()))
    {
        /* same suffix, so compare suffix version */
        if (ti == si)
        {
            if (not this->version().empty() and not that.version().empty())
                return ( strtoul(this->version()) == strtoul(that.version()) );
            else if (this->version().empty() and that.version().empty())
                return true;
            else
                return ( that.version().empty() ? false : true );
        }

        return ti == si;
    }
    else if ((ti != this->_suffixes.end()) or (si != this->_suffixes.end()))
        return false;

    return true;
}

/**********************
 * version_nosuffix_T *
 **********************/

void
portage::version_nosuffix_T::init(const std::string &PV)
{
    std::string::size_type pos = PV.find('_');
    if (pos != std::string::npos)
        this->_version = PV.substr(0, pos);
    else
        this->_version = PV;
}

bool
portage::version_nosuffix_T::operator< (version_nosuffix_T &that)
{
    bool differ = false;
    bool result = false;

    if (this->_version == that._version)
        return false;

    std::vector<std::string> thisparts = this->_version.split('.');
    std::vector<std::string> thatparts = that._version.split('.');
    std::vector<std::string>::size_type stoppos =
        std::min<std::vector<std::string>::size_type>(thisparts.size(),
                                                      thatparts.size());

    /* TODO: if thisparts.size() and thatpart.size() == 1, convert to long
     * and compare */

    std::vector<std::string>::iterator thisiter, thatiter;
    for (thisiter = thisparts.begin(), thatiter = thatparts.begin() ;
         stoppos != 0 ; ++thisiter, ++thatiter, --stoppos)
    {
        /* loop until the version components differ */

        uintmax_t thisver = strtouint(*thisiter);
        uintmax_t thatver = strtouint(*thatiter);

        bool same = false;
        if (thisver == thatver)
        {
            /* 1 == 01 ? they're the same in comparison speak but totally
             * not the same in version string speak */
            if (*thisiter == (std::string("0") + *thatiter))
                same = true;
            else
                continue;
        }
        
        result = ( same ? true : thisver < thatver );
        differ = true;
        break;
    }

    if (not differ)
        return thisparts.size() <= thatparts.size();

    return result;
}

bool
portage::version_nosuffix_T::operator== (version_nosuffix_T &that)
{
    /* string comparison should be sufficient for == */
    return this->_version == that._version;
}

/********************
 * version_string_T *
 ********************/

void
portage::version_string_T::init()
{
    this->split();
    this->_suffix.assign(this->_v["PVR"]);
    this->_version.assign(this->_v["PV"]);
}

/*
 * Display full version string.
 */

const std::string
portage::version_string_T::operator() () const
{
    /* chop -r0 if necessary */
    std::string::size_type pos = this->_verstr.rfind("-r0");
    if (pos != std::string::npos)
        return this->_verstr.substr(0, pos);

    return this->_verstr;
}

bool
portage::version_string_T::operator< (version_string_T &that)
{
    if (this->_version < that._version)
        return true;
    else if (this->_version == that._version)
    {
        if (this->_suffix < that._suffix)
            return true;
        else if (this->_suffix == that._suffix)
        {
            uintmax_t thisrev = strtouint(this->_v["PR"].substr(1).c_str());
            uintmax_t thatrev = strtouint(that["PR"].substr(1).c_str());
            return thisrev <= thatrev;
        }
    }

    return false;
}

bool
portage::version_string_T::operator== (version_string_T &that)
{
    return ( (this->_version == that._version) and
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

    assert(not this->_verstr.empty());

    /* append -r0 if necessary */
    if ((pos = this->_verstr.rfind("-r")) == std::string::npos)
        this->_verstr.append("-r0");

    parts = this->_verstr.split('-');

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
    this->_v["PN"] = comps[0];
    this->_v["PV"] = comps[1];
    this->_v["PR"] = comps[2];
    this->_v["P"]   = this->_v["PN"] + "-" + this->_v["PV"];
    this->_v["PVR"] = this->_v["PV"] + "-" + this->_v["PR"];
    this->_v["PF"]  = this->_v["PN"] + "-" + this->_v["PVR"];
}

/*****************
 * versions_T    *
 *****************/

portage::versions_T::versions_T(const util::path_T &path)
{
    util::dir_T pkgdir(path);
    util::dir_T::iterator d;
    
    for (d = pkgdir.begin() ; d != pkgdir.end() ; ++d)
        if (portage::is_ebuild(*d))
            assert(this->insert(*d));
}

portage::versions_T::iterator
portage::versions_T::find(const std::string &path)
{
    portage::version_string_T *v = new portage::version_string_T(path);
    portage::versions_T::iterator i = this->_vs.find(v);
    delete v;
    return i;
}

bool
portage::versions_T::insert(const util::path_T &path)
{
    portage::version_string_T *v = new portage::version_string_T(path);

//    std::cout << "versions_T::insert ===> trying to insert "
//        << (*v)() << std::endl;
    
    std::pair<iterator, bool> p = this->_vs.insert(v);
    
    if (not p.second)
        delete v;

//    std::cout << "versions_T::insert ===> successfully inserted "
//        << (*v)() << std::endl;

    return p.second;
}

portage::versions_T::~versions_T()
{
    for (iterator i = this->_vs.begin() ; i != this->_vs.end() ; ++i)
        delete *i;
}

/* vim: set tw=80 sw=4 et : */
