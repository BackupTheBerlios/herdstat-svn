/*
 * herdstat -- src/fields.hh
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

#ifndef _HAVE_FIELDS_HH
#define _HAVE_FIELDS_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <map>
#include <string>

#include <herdstat/util/regex.hh>
#include "options.hh"
#include "exceptions.hh"

/**
 * transform_fields_into_matches
 * for each object in the range [first,last), iterate over fields vector
 * comparing the criteria to the member function pointer mapped to the 
 * respective field.  Objects in the aforementioned range that match all
 * fields get written into OutputIterator.
 */

template <typename InputIterator,
          typename OutputIterator,
          typename UnaryOp>
void
transform_fields_into_matches(
        InputIterator first,
        InputIterator last,
        OutputIterator result,
        const fields_type& fields,
        const std::map<std::string,
            const std::string& (std::iterator_traits<InputIterator>::value_type::*)(void) const > & fm,
        UnaryOp op)
{
    typedef std::map<std::string, const std::string&
        (std::iterator_traits<InputIterator>::value_type::*)(void) const> fmap;
    
    util::Regex criteria;
    const int cflags(options::eregex() ?
        util::Regex::extended|util::Regex::icase : util::Regex::icase);

    for (; first != last ; ++first)
    {
        fields_type::const_iterator f;
        for (f = fields.begin() ; f != fields.end() ; ++f)
        {
            /* check if field is valid */
            typename fmap::const_iterator i = fm.find(f->first);
            if (i == fm.end())
                throw InvalidField(f->first);

            /* it's valid, so compile regex */
            criteria.assign(f->second, cflags);

            /* compare criteria against the return value of the
             * value_type member function mapped to this field. */
            typename std::iterator_traits<InputIterator>::value_type v(*first);
            if (criteria != (v.*(i->second))())
                break;

            /* we're on the last field, meaning all fields that came before
             * it also matched, so save it finally. */
            if ((f+1) == fields.end())
                *result++ = op(*first);
        }
    }
}

#endif /* _HAVE_FIELDS_HH */

/* vim: set tw=80 sw=4 et : */
