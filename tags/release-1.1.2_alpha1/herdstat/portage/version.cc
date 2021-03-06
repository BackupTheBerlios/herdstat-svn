/*
 * herdstat -- herdstat/portage/portage_version.cc
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

#include <locale>
#include <vector>
#include <iterator>
#include <memory>
#include <cstdlib>
#include <climits>
#include <cassert>

#include <herdstat/util/misc.hh>
#include <herdstat/util/string.hh>
#include <herdstat/portage/misc.hh>
#include <herdstat/portage/exceptions.hh>
#include <herdstat/portage/version.hh>

namespace portage {
/*** static members *********************************************************/
std::vector<std::string> version_string_T::suffix_T::_suffixes;
/****************************************************************************/
version_map_T::version_map_T(const std::string &path)
    : _verstr(util::chop_fileext(util::basename(path)))
{
    this->parse_verstr();
}
/****************************************************************************/
void
version_map_T::parse_verstr()
{
    /* append -r0 if necessary */
    std::string::size_type pos = this->_verstr.rfind("-r0");
    if (pos == std::string::npos)
    {
        pos = this->_verstr.rfind("-r");
        if ((pos == std::string::npos) or (((pos+2) <= this->_verstr.size()) and
            not std::isdigit(this->_verstr.at(pos+2))))
            this->_verstr.append("-r0");
    }

    std::vector<std::string> parts(util::split(this->_verstr, '-'));

    /* If parts > 3, ${PN} contains a '-' */
    if (parts.size() > 3)
    {
        std::string PN;
        while (parts.size() > 2)
        {
            PN += "-" + parts.front();
            parts.erase(parts.begin());
        }

        parts.insert(parts.begin(), PN);
    }

    /* this should NEVER != 3. */
    assert(parts.size() == 3);

    /* fill our map with the components */
    this->_vmap.insert(std::make_pair("PN", parts[0]));
    this->_vmap.insert(std::make_pair("PV", parts[1]));
    this->_vmap.insert(std::make_pair("PR", parts[2]));
    this->_vmap.insert(std::make_pair("P", (*this)["PN"] + "-" + (*this)["PV"]));
    this->_vmap.insert(std::make_pair("PVR", (*this)["PV"] + "-" + (*this)["PR"]));
    this->_vmap.insert(std::make_pair("PF", (*this)["PN"] + "-" + (*this)["PVR"]));
}
/*****************************************************************************
 * suffix_T                                                                  *
 *****************************************************************************/
void
version_string_T::suffix_T::init(const string_type &s)
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
}
/*****************************************************************************
 * Is this suffix less than that suffix?                                     *
 *****************************************************************************/
bool
version_string_T::suffix_T::operator< (suffix_T &that) const
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
                return ( util::destringify<unsigned long>(this->version()) <
                         util::destringify<unsigned long>(that.version()) );
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
version_string_T::suffix_T::operator== (suffix_T &that) const
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
                return ( util::destringify<unsigned long>(this->version()) ==
                         util::destringify<unsigned long>(that.version()) );
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
 * nosuffix_T                                                                *
 *****************************************************************************/
void
version_string_T::nosuffix_T::init(const string_type &s)
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
version_string_T::nosuffix_T::operator< (nosuffix_T &that) const
{
    bool differ = false;
    bool result = false;

    /* std::string comparison should be sufficient for == */
    if (*this == that)
        return false;
    else if (this->_version == that._version)
        return this->_extra < that._extra;

    std::vector<string_type> thisparts = util::split(this->_version, '.');
    std::vector<string_type> thatparts = util::split(that._version, '.');
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
        unsigned long thisver =
            util::destringify<unsigned long>(*thisiter);
        unsigned long thatver =
            util::destringify<unsigned long>(*thatiter);

        bool same = false;
        if (thisver == thatver)
        {
            /* 1 == 01 ? they're the same in comparison speak but 
             * absolutely not the same in version std::string speak */
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
version_string_T::nosuffix_T::operator== (nosuffix_T &that) const
{
    /* std::string comparison should be sufficient for == */
    return ((this->_version == that._version) and
            (this->_extra   == that._extra));
}
/*****************************************************************************
 * version_string_T                                                          *
 *****************************************************************************/
version_string_T::version_string_T(const std::string &path)
    : _ebuild(path), _v(path), _verstr(_v.version())
{
    this->_suffix.assign(this->_v["PVR"]);
    this->_version.assign(this->_v["PV"]);
}
/*****************************************************************************
 * Display full version std::string (as portage would).                           *
 *****************************************************************************/
version_string_T::operator
std::string() const
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
version_string_T::operator< (version_string_T &that) const
{
    if (this->_version < that._version)
        return true;
    else if (this->_version == that._version)
    {
        if (this->_suffix < that._suffix)
            return true;
        else if (this->_suffix == that._suffix)
        {
            unsigned long thisrev =
                util::destringify<unsigned long>(this->_v["PR"].substr(1).c_str());
            unsigned long thatrev =
                util::destringify<unsigned long>(that._v["PR"].substr(1).c_str());
            return thisrev <= thatrev;
        }
    }

    return false;
}
/*****************************************************************************
 * Is this version equal to that version?                                    *
 *****************************************************************************/
bool
version_string_T::operator== (version_string_T &that) const
{
    return ( (this->_version == that._version) and
             (this->_suffix == that._suffix) and
             (this->_v["PR"] == that._v["PR"]) );
}
/*****************************************************************************
 * versions_T                                                                *
 *****************************************************************************/
versions_T::versions_T(const std::vector<std::string> &paths)
{
    std::vector<std::string>::const_iterator i = paths.begin(),
                                              e = paths.end();
    for (; i != e ; ++i) this->append(*i);
}
/*****************************************************************************
 * Given a path to a package directory, insert a new version_string_T for    *
 * each ebuild found.  clear()'s container first.                            *
 *****************************************************************************/
void
versions_T::assign(const std::string &path)
{
    this->_vs.clear();

    if (not util::is_dir(path))
        return;

    const util::dir_T pkgdir(path);
    util::dir_T::const_iterator d = pkgdir.begin(), e = pkgdir.end();
    
    for (; d != e ; ++d)
    {
        if (is_ebuild(*d))
            assert(this->insert(*d));
    }
}
/*****************************************************************************
 * Same as assign() but does not call clear().                               *
 *****************************************************************************/
void
versions_T::append(const std::string &path)
{
    const util::dir_T pkgdir(path);
    util::dir_T::const_iterator d = pkgdir.begin(), e = pkgdir.end();
    
    for (; d != e ; ++d)
    {
        if (is_ebuild(*d))
            assert(this->insert(*d));
    }
}
/*****************************************************************************
 * find wrapper                                                              *
 *****************************************************************************/
versions_T::iterator
versions_T::find(const string_type &path)
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
versions_T::insert(const std::string &path)
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
versions_T::~versions_T()
{
    iterator i = this->_vs.begin(), e = this->_vs.end();
    for (; i != e ; ++i) delete *i;
}
/*****************************************************************************/
} // namespace portage

/* vim: set tw=80 sw=4 et : */
