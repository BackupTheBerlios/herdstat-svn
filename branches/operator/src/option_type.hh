/*
 * herdstat -- src/option_type.hh
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

#ifndef HAVE_OPTION_TYPE_HH
#define HAVE_OPTION_TYPE_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <ostream>
#include <algorithm>
#include <typeinfo>
#include <cstdlib>
#include "exceptions.hh"

/*
 * Generic type container for storing option values internally as their
 * actual type (bool, string, etc).
 */

class option_type_T
{
    public:
	option_type_T() : value(NULL) { }
	template<typename T> 
	option_type_T(const T &v) : value(new option_type_holder<T>(v)) { }
	option_type_T(const option_type_T &ot)
	    : value(ot.value ? ot.value->clone() : 0) { }
	~option_type_T() { delete value; }

	template<typename T>
	option_type_T &operator=(const T &t)
	{
	    option_type_T(t).swap(*this);
	    return *this;
	}

	option_type_T &operator=(const option_type_T &ot)
	{
	    option_type_T(ot).swap(*this);
	    return *this;
	}

	option_type_T &swap(option_type_T &ot)
	{
	    std::swap(value, ot.value);
	    return *this;
	}

	void dump(std::ostream &stream) const { value->dump(stream); }

	bool empty() const { return (not value); }

	const std::type_info &type() const 
	    { return value ? value->type() : typeid(void); }

	/* abstract base for option_type_holder */
	class option_type_holder_base
	{
	    public:
		virtual ~option_type_holder_base() { }
		virtual const std::type_info &type() const = 0;
		virtual option_type_holder_base *clone() const = 0;
		virtual void dump(std::ostream &stream) const = 0;
	};

	template<typename T>
	class option_type_holder : public option_type_holder_base
	{
	    public:
		option_type_holder(const T &val) : v(val) { }
		virtual const std::type_info &type() const
		    { return typeid(T); }
		virtual option_type_holder_base *clone() const
		    { return new option_type_holder(v); }
		virtual void dump(std::ostream &stream) const
		    { stream << v; }
		T v;
	};

	option_type_holder_base *value;
};

template<typename T>
T *option_cast(option_type_T *opt)
{
    return opt && opt->type() == typeid(T) ?
	&static_cast<option_type_T::option_type_holder<T> *>(opt->value)->v : 0;
}

template<typename T>
const T *option_cast(const option_type_T *opt)
{
    return option_cast<T>(const_cast<option_type_T * >(opt));
}

template<typename T>
T option_cast(const option_type_T &opt)
{
    const T *result = option_cast<T>(&opt);
    if (not result)
	throw bad_option_cast_E();
    return *result;
}

#endif

/* vim: set tw=80 sw=4 et : */
