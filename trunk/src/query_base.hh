/*
 * herdstat -- src/query_base.hh
 * $Id: query_results.hh 827 2005-11-06 13:10:28Z ka0ttic $
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

#ifndef _HAVE_QUERY_BASE_HH
#define _HAVE_QUERY_BASE_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string>
#include <utility>

#include <herdstat/util/string.hh>
#include <herdstat/util/container_base.hh>

class QueryBase
    : public herdstat::util::VectorBase<std::pair<std::string, std::string> >
{
    public:
        virtual ~QueryBase() { }

        ///@{
        /// Add a string.
        inline void add(const std::string& val);
        inline void add(const char * const val);
        ///@}

        ///@{
        /// Add a field and a string.
        inline void add(const std::string& field, const std::string& val);
        inline void add(const char * const field, const char * const val);
        inline void add(const char * const field, const std::string& val);
        inline void add(const std::string& field, const char * const val);
        inline void add(const std::pair<std::string, std::string>& p);
        ///@}

        /// Add a field and a type.
        template <typename T>
        inline void add(const std::string& field, const T& val);
        /// Add a type with an empty field.
        template <typename T>
        inline void add(const T& val);

        ///@{
        /// Add a range [first,last).
        template <typename InputIterator>
        inline void add(InputIterator first, InputIterator last);
        template <typename InputIterator>
        void add(const std::string& field, InputIterator first,
                 InputIterator last);
        ///@}

        ///@{
        /// Add the result of UnaryOp() run on each in the range [first,last).
        template <typename InputIterator, typename UnaryOp>
        inline void transform(InputIterator first, InputIterator last,
                              UnaryOp op);
        template <typename InputIterator, typename UnaryOp>
        void transform(const std::string& field, InputIterator first,
                 InputIterator last, UnaryOp op);
        ///@}
        
        ///@{
        /// Add each in the range [first,last) for which UnaryPred returns true.
        template <typename InputIterator, typename UnaryPred>
        inline void add_if(InputIterator first, InputIterator last,
                           UnaryPred pred);
        template <typename InputIterator, typename UnaryPred>
        void add_if(const std::string& field,
                    InputIterator first, InputIterator last, UnaryPred pred);
        ///@}
        
        ///@{
        /** Add the result of UnaryOp() for each in the range [first,last) for
         *  which UnaryPred returns true.
         */
        template <typename InputIterator, typename UnaryPred, typename UnaryOp>
        inline void transform_if(InputIterator first, InputIterator last,
                                 UnaryPred pred, UnaryOp op);
        template <typename InputIterator, typename UnaryPred, typename UnaryOp>
        void transform_if(const std::string& field, InputIterator first,
                          InputIterator last, UnaryPred pred, UnaryOp op);
        ///@}
};

inline void
QueryBase::add(const std::string& val)
{
    this->add(std::string(), val);
}

inline void
QueryBase::add(const std::string& field, const std::string& val)
{
    this->push_back(std::make_pair(field, val));
}

inline void
QueryBase::add(const char * const val)
{
    this->add(std::string(), std::string(val));
}

inline void
QueryBase::add(const char * const field, const char * const val)
{
    this->add(std::string(field), std::string(val));
}

inline void
QueryBase::add(const char * const field, const std::string& val)
{
    this->add(std::string(field), val);
}

inline void
QueryBase::add(const std::string& field, const char * const val)
{
    this->add(field, std::string(val));
}

inline void
QueryBase::add(const std::pair<std::string, std::string>& p)
{
    this->add(p.first, p.second);
}

template <typename T>
inline void
QueryBase::add(const std::string& field, const T& val)
{
    this->add(field, herdstat::util::stringify<T>(val));
}

template <typename T>
inline void
QueryBase::add(const T& val)
{
    this->add(std::string(), val);
}

template <typename InputIterator>
inline void
QueryBase::add(InputIterator first, InputIterator last)
{
    this->add(std::string(), first, last);
}

template <typename InputIterator>
void
QueryBase::add(const std::string& field,
               InputIterator first,
               InputIterator last)
{
    std::string val;
    while (first != last)
    {
        val += *first;
        if (++first != last)
            val += " ";
    }

    this->add(field, val);
}

template <typename InputIterator, typename UnaryOp>
inline void
QueryBase::transform(InputIterator first,
                     InputIterator last,
                     UnaryOp op)
{
    this->transform(std::string(), first, last, op);
}

template <typename InputIterator, typename UnaryOp>
void
QueryBase::transform(const std::string& field,
                     InputIterator first,
                     InputIterator last,
                     UnaryOp op)
{
    std::string val;
    while (first != last)
    {
        val += op(*first);
        if (++first != last)
            val += " ";
    }

    this->add(field, val);
}

template <typename InputIterator, typename UnaryPred>
inline void
QueryBase::add_if(InputIterator first,
                  InputIterator last,
                  UnaryPred pred)
{
    this->add_if(std::string(), first, last, pred);
}

template <typename InputIterator, typename UnaryPred>
void
QueryBase::add_if(const std::string& field,
                  InputIterator first,
                  InputIterator last,
                  UnaryPred pred)
{
    std::string val;
    while (first != last)
    {
        if (pred(*first))
        {
            val += *first;
            if (++first != last)
                val += " ";
        }
        else ++first;
    }

    this->add(field, val);
}

template <typename InputIterator, typename UnaryPred, typename UnaryOp>
inline void
QueryBase::transform_if(InputIterator first,
                        InputIterator last,
                        UnaryPred pred,
                        UnaryOp op)
{
    this->transform_if(std::string(), first, last, pred, op);
}

template <typename InputIterator, typename UnaryPred, typename UnaryOp>
void
QueryBase::transform_if(const std::string& field,
                        InputIterator first,
                        InputIterator last,
                        UnaryPred pred,
                        UnaryOp op)
{
    std::string val;
    while (first != last)
    {
        if (pred(*first))
        {
            val += op(*first);
            if (++first != last)
                val += " ";
        }
        else ++first;
    }

    this->add(field, val);
}

template <typename T, typename InputIterator>
void
copy_to_query(InputIterator first,
              InputIterator last,
              T& v)
{
    while (first != last)
        v.add(*first++);
}

template <typename T, typename InputIterator, typename UnaryOp>
void
transform_to_query(InputIterator first,
                   InputIterator last,
                   T& v,
                   UnaryOp op)
{
    while (first != last)
        v.add(op(*first++));
}

template <typename T, typename InputIterator, typename UnaryPred>
void
copy_to_query_if(InputIterator first,
                 InputIterator last,
                 T& v,
                 UnaryPred pred)
{
    for ( ; first != last ; ++first)
        if (pred(*first))
            v.add(*first);
}

template <typename T, typename InputIterator,
          typename UnaryPred, typename UnaryOp>
void
transform_to_query_if(InputIterator first,
                      InputIterator last,
                      T& v,
                      UnaryPred pred,
                      UnaryOp op)
{
    for ( ; first != last ; ++first)
        if (pred(*first))
            v.add(op(*first));
}

#endif /* _HAVE_QUERY_BASE_HH */

/* vim: set tw=80 sw=4 fdm=marker et : */
