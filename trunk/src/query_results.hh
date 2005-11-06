/*
 * herdstat -- src/query_results.hh
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

#ifndef _HAVE_QUERY_RESULTS_HH
#define _HAVE_QUERY_RESULTS_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string>
#include <utility>

#include <herdstat/util/string.hh>
#include <herdstat/util/container_base.hh>

class QueryResults
    : public herdstat::util::VectorBase<std::pair<std::string, std::string> >
{
    public:
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

        /// Add an empty line.
        inline void add_linebreak();
};

inline void
QueryResults::add_linebreak()
{
    this->add(std::string());
}

inline void
QueryResults::add(const std::string& val)
{
    this->add(std::string(), val);
}

inline void
QueryResults::add(const std::string& field, const std::string& val)
{
    this->push_back(std::make_pair(field, val));
}

inline void
QueryResults::add(const char * const val)
{
    this->add(std::string(), std::string(val));
}

inline void
QueryResults::add(const char * const field, const char * const val)
{
    this->add(std::string(field), std::string(val));
}

inline void
QueryResults::add(const char * const field, const std::string& val)
{
    this->add(std::string(field), val);
}

inline void
QueryResults::add(const std::string& field, const char * const val)
{
    this->add(field, std::string(val));
}

inline void
QueryResults::add(const std::pair<std::string, std::string>& p)
{
    this->add(p.first, p.second);
}

template <typename T>
inline void
QueryResults::add(const std::string& field, const T& val)
{
    this->add(field, herdstat::util::stringify<T>(val));
}

template <typename T>
inline void
QueryResults::add(const T& val)
{
    this->add(std::string(), val);
}

template <typename InputIterator>
inline void
QueryResults::add(InputIterator first, InputIterator last)
{
    this->add(std::string(), first, last);
}

template <typename InputIterator>
void
QueryResults::add(const std::string& field,
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
QueryResults::transform(InputIterator first,
                        InputIterator last,
                        UnaryOp op)
{
    this->transform(std::string(), first, last, op);
}

template <typename InputIterator, typename UnaryOp>
void
QueryResults::transform(const std::string& field,
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
QueryResults::add_if(InputIterator first,
                     InputIterator last,
                     UnaryPred pred)
{
    this->add_if(std::string(), first, last, pred);
}

template <typename InputIterator, typename UnaryPred>
void
QueryResults::add_if(const std::string& field,
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
QueryResults::transform_if(InputIterator first,
                     InputIterator last,
                     UnaryPred pred,
                     UnaryOp op)
{
    this->transform_if(std::string(), first, last, pred, op);
}

template <typename InputIterator, typename UnaryPred, typename UnaryOp>
void
QueryResults::transform_if(const std::string& field,
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

template <typename InputIterator>
void
copy_to_results(InputIterator first,
                InputIterator last,
                QueryResults& results)
{
    while (first != last)
        results.add(*first++);
}

template <typename InputIterator, typename UnaryOp>
void
transform_to_results(InputIterator first,
                     InputIterator last,
                     QueryResults& results,
                     UnaryOp op)
{
    while (first != last)
        results.add(op(*first++));
}

template <typename InputIterator, typename UnaryPred>
void
copy_to_results_if(InputIterator first,
                   InputIterator last,
                   QueryResults &results,
                   UnaryPred pred)
{
    for ( ; first != last ; ++first)
        if (pred(*first))
            results.add(*first);
}

template <typename InputIterator, typename UnaryPred, typename UnaryOp>
void
transform_to_results_if(InputIterator first,
                        InputIterator last,
                        QueryResults& results,
                        UnaryPred pred,
                        UnaryOp op)
{
    for ( ; first != last ; ++first)
        if (pred(*first))
            results.add(op(*first));
}

#endif /* _HAVE_QUERY_RESULTS_HH */

/* vim: set tw=80 sw=4 fdm=marker et : */
