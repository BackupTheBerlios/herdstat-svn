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
        ///@}

        /// Add a field and a type.
        template <typename T>
        inline void add(const std::string& field, const T& val);
        /// Add a type with an empty field.
        template <typename T>
        inline void add(const T& val);

        ///@{
        /// Add a range [begin,end).
        template <typename InIter>
        inline void add(InIter begin, InIter end);
        template <typename InIter>
        void add(const std::string& field, InIter begin, InIter end);
        ///@}

        ///@{
        /// Add the result of UnaryOp() run on each in the range [begin,end).
        template <typename InIter, typename UnaryOp>
        inline void add(InIter begin, InIter end, UnaryOp op);
        template <typename InIter, typename UnaryOp>
        void add(const std::string& field, InIter begin, InIter end, UnaryOp op);
        ///@}
        
        ///@{
        /// Add each in the range [begin,end) for which UnaryPred returns true.
        template <typename InIter, typename UnaryPred>
        inline void add_if(InIter begin, InIter end, UnaryPred pred);
        template <typename InIter, typename UnaryPred>
        void add_if(const std::string& field,
                    InIter begin, InIter end, UnaryPred pred);
        ///@}
        
        ///@{
        /** Add the result of UnaryOp() for each in the range [begin,end) for
         *  which UnaryPred returns true.
         */
        template <typename InIter, typename UnaryPred, typename UnaryOp>
        inline void add_if(InIter begin, InIter end, UnaryPred pred, UnaryOp op);
        template <typename InIter, typename UnaryPred, typename UnaryOp>
        void add_if(const std::string& field, InIter begin, InIter end,
                    UnaryPred pred, UnaryOp op);
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

template <typename InIter>
inline void
QueryResults::add(InIter begin, InIter end)
{
    this->add(std::string(), begin, end);
}

template <typename InIter>
void
QueryResults::add(const std::string& field, InIter begin, InIter end)
{
    std::string val;
    while (begin != end)
    {
        val += *begin;
        if (++begin != end)
            val += " ";
    }

    this->add(field, val);
}

template <typename InIter, typename UnaryOp>
inline void
QueryResults::add(InIter begin, InIter end, UnaryOp op)
{
    this->add(std::string(), begin, end, op);
}

template <typename InIter, typename UnaryOp>
void
QueryResults::add(const std::string& field, InIter begin, InIter end, UnaryOp op)
{
    std::string val;
    while (begin != end)
    {
        val += op(*begin);
        if (++begin != end)
            val += " ";
    }

    this->add(field, val);
}

template <typename InIter, typename UnaryPred>
inline void
QueryResults::add_if(InIter begin, InIter end, UnaryPred pred)
{
    this->add(std::string(), begin, end, pred);
}

template <typename InIter, typename UnaryPred>
void
QueryResults::add_if(const std::string& field,
                     InIter begin, InIter end, UnaryPred pred)
{
    std::string val;
    while (begin != end)
    {
        if (pred(*begin))
        {
            val += *begin;
            if (++begin != end)
                val += " ";
        }
        else ++begin;
    }
}

template <typename InIter, typename UnaryPred, typename UnaryOp>
inline void
QueryResults::add_if(InIter begin, InIter end, UnaryPred pred, UnaryOp op)
{
    this->add(std::string(), begin, end, pred, op);
}

template <typename InIter, typename UnaryPred, typename UnaryOp>
void
QueryResults::add_if(const std::string& field, InIter begin, InIter end,
                     UnaryPred pred, UnaryOp op)
{
    std::string val;
    while (begin != end)
    {
        if (pred(*begin))
        {
            val += op(*begin);
            if (++begin != end)
                val += " ";
        }
        else ++begin;
    }
}

#endif /* _HAVE_QUERY_RESULTS_HH */

/* vim: set tw=80 sw=4 fdm=marker et : */
