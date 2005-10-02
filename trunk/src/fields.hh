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
#include <utility>
#include <string>

#include <herdstat/util/regex.hh>
#include "options.hh"
#include "exceptions.hh"

/**
 * Convenience class for users of transform_fields_into_matches.
 * Represents a map that maps strings to member functions
 * of type T, used in comparison against the field criteria.
 * Only offers a small subset of std::map's interface.
 */

template <typename T>
class FieldsMap
{
    public:
        /* maps strings to member functions pointers of T */
        typedef const std::string& (T::*mfp)(void) const;
        typedef std::map<std::string, mfp> container_type;
        typedef typename container_type::iterator iterator;
        typedef typename container_type::const_iterator const_iterator;
        typedef typename container_type::key_type key_type;
        typedef typename container_type::mapped_type mapped_type;
        typedef typename container_type::value_type value_type;
        typedef typename container_type::reference reference;
        typedef typename container_type::const_reference const_reference;

        iterator begin() { return _mfps.begin(); }
        const_iterator begin() const { return _mfps.begin(); }
        iterator end() { return _mfps.end(); }
        const_iterator end() const { return _mfps.end(); }

        mapped_type& operator[] (const key_type& k) { return _mfps[k]; }
        iterator find(const key_type& k) { return _mfps.find(k); }
        const_iterator find(const key_type& k) const { return _mfps.find(k); }

        std::pair<iterator, bool> insert(const key_type& k, const mapped_type& v)
        { return _mfps.insert(std::make_pair(k, v)); }

        /* returns comma-separated list of keys */
        const std::string keys() const
        {
            std::string result;
            for (const_iterator i = this->begin() ; i != this->end() ; ++i)
                result += i->first + ",";
            return result.substr(0, result.length() - 1);
        }

    private:
        container_type _mfps;
};

/**
 * transform_fields_into_matches
 * for each object in the range [first,last), iterate over fields vector,
 * generating a regular expression for each field value, comparing it to
 * the return value of the correspoding member function (member function of
 * the type that InIter points to).  If each field's criteria is met for
 * that iteration of [first,last), UnaryOp is called on the iterated object,
 * saving the result into OutIter.
 *
 * @param first Input iterator.
 * @param last  Input iterator.
 * @param result Output iterator.
 * @param cmp Comparison type (that is compared against the function pointer
 * mapped to the corresponding field).
 * @param fields const reference to a fields_type object.
 * @param fm const reference to a FieldsMap object that maps allowed fields to
 * their corresponding member function pointer.
 * @param op Unary functor that returns a const std::string&.
 */

/* implicit interface assumes:
 *   CmpType:
 *      - provides an assign(const std::string&) member
 *      - provides an operator!=(const std::string&) member
 *   InIter:
 *      - provides member functions corresponding to member function
 *        pointers mapped in FieldsMap
 *   OutIter:
 *      - Forward output iterator that points to a std::string.
 *   UnaryOp:
 *      - takes a single parameter of the type InIter points to
 *      - returns a const std::string&
 */

template <typename InIter, typename OutIter, typename CmpType, typename UnaryOp>
void
transform_fields_into_matches(InIter first, InIter last, OutIter result,
        CmpType& cmp, const fields_type& fields,
        const FieldsMap<typename std::iterator_traits<InIter>::value_type>& fm,
        UnaryOp op)
{
    /* type that InIter points to */
    typedef typename std::iterator_traits<InIter>::value_type InIterT;

    for (; first != last ; ++first)
    {
        fields_type::const_iterator f;
        for (f = fields.begin() ; f != fields.end() ; ++f)
        {
            /* check if field is valid (exists) */
            typename FieldsMap<InIterT>::const_iterator i = fm.find(f->first);
            if (i == fm.end())
                throw InvalidField(f->first);

            /* it's valid, so assign comparison value */
            cmp.assign(f->second);

            /* compare criteria against the return value of the
             * value_type member function mapped to this field. */
            InIterT v(*first);
            if (cmp != (v.*(i->second))())
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
