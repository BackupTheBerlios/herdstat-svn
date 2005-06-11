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

//#include <iostream>
#include <vector>
#include <iterator>
#include <memory>
#include <cstdlib>
#include <climits>
#include <cassert>

#ifdef UNICODE
# include <glibmm/unicode.h>
#else /* UNICODE */
# include <locale>
#endif /* UNICODE */

#include "misc.hh"
#include "string.hh"
#include "portage_misc.hh"
#include "portage_exceptions.hh"
#include "portage_version.hh"

std::vector<util::string> portage::version_string_T::suffix_T::_suffixes;

/*****************************************************************************
 * version_suffix_T                                                          *
 *****************************************************************************/
void
portage::version_string_T::suffix_T::init(const string_type &s)
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
/*****************************************************************************
 * Given ${PVR}, retrieve the suffix and suffix version.                     *
 *****************************************************************************/
void
portage::version_string_T::suffix_T::get_suffix(const string_type &s)
{
    string_type result(s);
    string_type::size_type pos = result.rfind("-r");
    if (pos != string_type::npos)
        result = result.substr(0, pos);

    if ((pos = result.rfind('_')) != string_type::npos)
    {
        this->_suffix = result.substr(pos + 1);
        
        /* chop any trailing suffix version */
        pos = this->_suffix.find_first_of("0123456789");
        if (pos != string_type::npos)
        {
            this->_suffix_ver = this->_suffix.substr(pos);
            this->_suffix = this->_suffix.substr(0, pos);
        }

        /* valid suffix? */
        if (std::find(this->_suffixes.begin(), this->_suffixes.end(),
            this->_suffix) == this->_suffixes.end())
            this->_suffix.clear();
    }

//    util::debug("suffix == '%s' suffix version == '%s'",
//            _suffix.c_str(), _suffix_ver.c_str());
}
/*****************************************************************************
 * Is this suffix less than that suffix?                                     *
 *****************************************************************************/
bool
portage::version_string_T::suffix_T::operator< (suffix_T &that)
{
    std::vector<string_type>::iterator ti, si;

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
                return ( util::strtoul(this->version()) < 
                         util::strtoul(that.version()) );
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
/*****************************************************************************
 * Is this suffix equal to that suffix?                                      *
 *****************************************************************************/
bool
portage::version_string_T::suffix_T::operator== (suffix_T &that)
{
    std::vector<string_type>::iterator ti, si;

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
                return ( util::strtoul(this->version()) ==
                         util::strtoul(that.version()) );
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
/*****************************************************************************
 * version_nosuffix_T                                                        *
 *****************************************************************************/
void
portage::version_string_T::nosuffix_T::init(const string_type &s)
{
    string_type PV(s);

    /* strip suffix */
    string_type::size_type pos;
    if ((pos = PV.find('_')) != string_type::npos)
        PV = PV.substr(0, pos);

    if ((pos = PV.find_first_not_of("0123456789.")) != string_type::npos)
    {
        this->_extra = PV.substr(pos);
        PV = PV.substr(0, pos);
    }

    this->_version = PV;
}
/*****************************************************************************
 * Is this version (minus suffix) less that that version (minus suffix)?     *
 *****************************************************************************/
bool
portage::version_string_T::nosuffix_T::operator< (nosuffix_T &that)
{
    bool differ = false;
    bool result = false;

    /* string comparison should be sufficient for == */
    if (*this == that)
        return false;
    else if (this->_version == that._version)
        return this->_extra < that._extra;

    std::vector<string_type> thisparts = this->_version.split('.');
    std::vector<string_type> thatparts = that._version.split('.');
    std::vector<string_type>::size_type stoppos =
        std::min<std::vector<string_type>::size_type>(thisparts.size(),
                                                       thatparts.size());

    /* TODO: if thisparts.size() and thatpart.size() == 1, convert to long
     * and compare */

    std::vector<string_type>::iterator thisiter, thatiter;
    for (thisiter = thisparts.begin(), thatiter = thatparts.begin() ;
         stoppos != 0 ; ++thisiter, ++thatiter, --stoppos)
    {
        /* loop until the version components differ */

        /* TODO: use std::mismatch() ?? */

        uintmax_t thisver = util::strtouint(*thisiter);
        uintmax_t thatver = util::strtouint(*thatiter);

        bool same = false;
        if (thisver == thatver)
        {
            /* 1 == 01 ? they're the same in comparison speak but 
             * absolutely not the same in version string speak */
            if (*thisiter == (string_type("0") + *thatiter))
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
/*****************************************************************************
 * Is this version (minus suffix) equal to that version (minus suffix)?      *
 *****************************************************************************/
bool
portage::version_string_T::nosuffix_T::operator== (nosuffix_T &that)
{
    /* string comparison should be sufficient for == */
    return ((this->_version == that._version) and
            (this->_extra   == that._extra));
}
/*****************************************************************************
 * version_string_T                                                          *
 *****************************************************************************/
void
portage::version_string_T::init()
{
    this->parse();
    this->_suffix.assign(this->_v["PVR"]);
    this->_version.assign(this->_v["PV"]);
}
/*****************************************************************************
 * Display full version string (as portage would).                           *
 *****************************************************************************/
const portage::version_string_T::string_type
portage::version_string_T::operator() () const
{
    /* chop -r0 if necessary */
    string_type::size_type pos = this->_verstr.rfind("-r0");
    if (pos != string_type::npos)
        return this->_verstr.substr(0, pos);

    return this->_verstr;
}
/*****************************************************************************
 * Is this version less than that version?                                   *
 *****************************************************************************/
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
            uintmax_t thisrev =
                util::strtouint(this->_v["PR"].substr(1).c_str());
            uintmax_t thatrev =
                util::strtouint(that["PR"].substr(1).c_str());
            return thisrev <= thatrev;
        }
    }

    return false;
}
/*****************************************************************************
 * Is this version equal to that version?                                    *
 *****************************************************************************/
bool
portage::version_string_T::operator== (version_string_T &that)
{
    return ( (this->_version == that._version) and
             (this->_suffix == that._suffix) and
             (this->_v["PR"] == that["PR"]) );
}
/*****************************************************************************
 * Split full version string into components ${P}, ${PV}, ${PN}, etc and     *
 * save each one in our internal map.                                        *
 *****************************************************************************/
void
portage::version_string_T::parse()
{
    string_type::size_type pos;
    std::vector<string_type> parts;

    assert(not this->_verstr.empty());

    /* append -r0 if necessary */
    pos = this->_verstr.rfind("-r");

#ifdef UNICODE
    if ((pos == string_type::npos) or (((pos+2) <= this->_verstr.size()) and
        not Glib::Unicode::isdigit(this->_verstr.at(pos+2))))
#else /* UNICODE */
    if ((pos == string_type::npos) or (((pos+2) <= this->_verstr.size()) and
        not std::isdigit(this->_verstr.at(pos+2), std::locale(""))))
#endif /* UNICODE */

    {
        this->_verstr.append("-r0");
    }

    parts = this->_verstr.split('-');

    /* If parts > 3, ${PN} contains a '-' */
    if (parts.size() > 3)
    {
        string_type PN;
        while (parts.size() > 2)
        {
            PN += "-" + parts.front();
            parts.erase(parts.begin());
        }

        parts.insert(parts.begin(), PN);
    }

    assert(parts.size() == 3);

    /* fill our map with the components */
    this->_v["PN"] = parts[0];
    this->_v["PV"] = parts[1];
    this->_v["PR"] = parts[2];
    this->_v["P"]   = this->_v["PN"] + "-" + this->_v["PV"];
    this->_v["PVR"] = this->_v["PV"] + "-" + this->_v["PR"];
    this->_v["PF"]  = this->_v["PN"] + "-" + this->_v["PVR"];
}
/*****************************************************************************
 * versions_T                                                                *
 *****************************************************************************/
portage::versions_T::versions_T(const std::vector<util::path_T> &paths)
{
    std::vector<util::path_T>::const_iterator i = paths.begin(),
                                              e = paths.end();
    for (; i != e ; ++i) this->append(*i);
}
/*****************************************************************************
 * Given a path to a package directory, insert a new version_string_T for    *
 * each ebuild found.  clear()'s container first.                            *
 *****************************************************************************/
void
portage::versions_T::assign(const util::path_T &path)
{
    this->_vs.clear();

    if (not util::is_dir(path))
        return;

    const util::dir_T pkgdir(path);
    util::dir_T::const_iterator d = pkgdir.begin(), e = pkgdir.end();
    
    for (; d != e ; ++d)
    {
        if (portage::is_ebuild(*d))
            assert(this->insert(*d));
    }
}
/*****************************************************************************
 * Same as assign() but does not call clear().                               *
 *****************************************************************************/
void
portage::versions_T::append(const util::path_T &path)
{
    const util::dir_T pkgdir(path);
    util::dir_T::const_iterator d = pkgdir.begin(), e = pkgdir.end();
    
    for (; d != e ; ++d)
    {
        if (portage::is_ebuild(*d))
            assert(this->insert(*d));
    }
}
/*****************************************************************************
 * find wrapper                                                              *
 *****************************************************************************/
portage::versions_T::iterator
portage::versions_T::find(const string_type &path)
{
    version_string_T *v = new version_string_T(path);
    versions_T::iterator i = this->_vs.find(v);
    delete v;
    return i;
}
/*****************************************************************************
 * insert wrapper                                                            *
 *****************************************************************************/
bool
portage::versions_T::insert(const util::path_T &path)
{
    version_string_T *v = new version_string_T(path);

//    std::cout << "versions_T::insert ===> trying to insert "
//        << (*v)() << std::endl;
    
    std::pair<iterator, bool> p = this->_vs.insert(v);
    
    if (not p.second)
        delete v;

//    std::cout << "versions_T::insert ===> successfully inserted "
//        << (*v)() << std::endl;

    return p.second;
}
/*****************************************************************************
 * clean up                                                                  *
 *****************************************************************************/
portage::versions_T::~versions_T()
{
    iterator i = this->_vs.begin(), e = this->_vs.end();
    for (; i != e ; ++i) delete *i;
}
/*****************************************************************************/

/* vim: set tw=80 sw=4 et : */
