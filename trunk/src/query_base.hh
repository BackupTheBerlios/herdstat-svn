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

class QuerySpec
{
    public:
        typedef std::string first_type;
        typedef std::string second_type;

        QuerySpec(const second_type& val)
            : first(), second(val) { }
        QuerySpec(const first_type& field, const second_type& val)
            : first(field), second(val) { }
        QuerySpec(const std::pair<first_type, second_type>& p)
            : first(p.first), second(p.second) { }

        operator std::pair<first_type, second_type>() const
        { return std::pair<first_type, second_type>(first, second); }

        QuerySpec& operator= (const second_type& val)
        { second.assign(val); return *this; }
        QuerySpec& operator= (const std::pair<first_type, second_type>& p)
        { first.assign(p.first) ; second.assign(p.second) ; return *this; }

        bool operator== (const std::pair<first_type, second_type>& p)
        { return ((p.first == first) and (p.second == second)); }
        bool operator!= (const std::pair<first_type, second_type>& p)
        { return not (*this == p); }

        first_type first;
        second_type second;
};

class QueryBase
{
    public:
        typedef std::vector<QuerySpec> container_type;
        typedef container_type::value_type value_type;
        typedef container_type::size_type size_type;
        typedef container_type::iterator iterator;
        typedef container_type::const_iterator const_iterator;
        typedef container_type::reference reference;
        typedef container_type::const_reference const_reference;

        virtual ~QueryBase() { }

        inline container_type& operator= (const container_type& c);

        inline size_type size() const;
        inline bool empty() const;
        inline void clear();

        inline iterator begin();
        inline const_iterator begin() const;
        inline iterator end();
        inline const_iterator end() const;

        inline reference front();
        inline const_reference front() const;
        inline reference back();
        inline const_reference back() const;

        inline void push_back(const std::string& str);
        inline void push_back(const value_type& v);
        inline void push_back(const std::pair<std::string, std::string>& p);

        inline iterator insert(iterator hpos, const value_type& v);
        template <typename InputIterator>
        inline iterator insert(InputIterator first, InputIterator last);

        inline iterator erase(iterator pos);
        inline iterator erase(iterator first, iterator last);

        template <typename InputIterator>
        inline void assign(InputIterator first, InputIterator last);

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
        inline void add(const value_type& p);
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
    
    protected:
        container_type& container() { return _query; }
        const container_type& container() const { return _query; }

    private:
        container_type _query;
};

inline QueryBase::container_type&
QueryBase::operator=(const container_type& c)
{
    return _query = c;
}

inline QueryBase::size_type
QueryBase::size() const
{
    return _query.size();
}

inline bool
QueryBase::empty() const
{
    return _query.empty();
}

inline void
QueryBase::clear()
{
    _query.clear();
}

inline QueryBase::iterator
QueryBase::begin()
{
    return _query.begin();
}

inline QueryBase::const_iterator
QueryBase::begin() const
{
    return _query.begin();
}

inline QueryBase::iterator
QueryBase::end()
{
    return _query.end();
}

inline QueryBase::const_iterator
QueryBase::end() const
{
    return _query.end();
}

inline QueryBase::reference
QueryBase::front()
{
    return _query.front();
}

inline QueryBase::const_reference
QueryBase::front() const
{
    return _query.front();
}

inline QueryBase::reference
QueryBase::back()
{
    return _query.back();
}

inline QueryBase::const_reference
QueryBase::back() const
{
    return _query.back();
}

inline void
QueryBase::push_back(const std::string& val)
{
    this->add(val);
}

inline void
QueryBase::push_back(const value_type& v)
{
    _query.push_back(v);
}

inline void
QueryBase::push_back(const std::pair<std::string, std::string>& p)
{
    _query.push_back(p);
}

inline QueryBase::iterator
QueryBase::insert(iterator hpos, const value_type& v)
{
    return _query.insert(hpos, v);
}

template <typename InputIterator>
inline QueryBase::iterator
QueryBase::insert(InputIterator first, InputIterator last)
{
    this->add(first, last);
    return (this->end() - 1);
}

inline QueryBase::iterator
QueryBase::erase(iterator pos)
{
    return _query.erase(pos);
}

inline QueryBase::iterator
QueryBase::erase(iterator first, iterator last)
{
    return _query.erase(first, last);
}

template <typename InputIterator>
inline void
QueryBase::assign(InputIterator first, InputIterator last)
{
    _query.assign(first, last);
}

inline void
QueryBase::add(const std::string& val)
{
    this->add(std::string(), val);
}

inline void
QueryBase::add(const std::string& field, const std::string& val)
{
    _query.push_back(value_type(field, val));
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
QueryBase::add(const value_type& v)
{
    _query.push_back(v);
}

inline void
QueryBase::add(const std::pair<std::string, std::string>& p)
{
    _query.push_back(p);
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

#endif /* _HAVE_QUERY_BASE_HH */

/* vim: set tw=80 sw=4 fdm=marker et : */
