/*
 * herdstat -- src/options.hh
 * $Id$
 * Copyright (c) 2005 Aaron Walker <ka0ttic at gentoo.org>
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

#ifndef HAVE_OPTIONS_HH
#define HAVE_OPTIONS_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <ostream>
#include <string>
#include <map>
#include "option_type.hh"

enum options_action_T
{
    action_unspecified,
    action_herd,
    action_dev,
    action_pkg,
    action_stats
};

class options_T
{
    private:
	class option_map_T : public std::map<std::string, option_type_T * >
	{
	    public:
		option_map_T() { set_defaults(); }
		~option_map_T()
		{
		    for (iterator i = begin() ; i != end() ; ++i)
			delete i->second;
		}

		/* set_defaults() is defined in the source file ; that way the only
		 * thing we have to do when adding a new option, is set it there. */
		void set_defaults();
	};

	static option_map_T optmap;

    public:
	/* get option with specified name */
	template<typename T>
	static const T get(std::string const &id)
	{
	    if (not optmap[id])
		throw invalid_option_E(id);
	    return option_cast<T>(*optmap[id]);
	}

	/* set specified option name to specified value */
	template<typename T>
	static void set(std::string const &id, const T &t)
	{
	    option_map_T::iterator i = optmap.find(id);
	    if (i != optmap.end())
	    {
		delete i->second;
		optmap.erase(i);
	    }
            optmap[id] = new option_type_T(t);
	}

	/* dump all the options to the specified stream */
	static void dump(std::ostream &stream)
	{
	    stream << "******************** options ********************" << std::endl;
	    option_map_T::iterator i;
	    for (i = optmap.begin() ; i != optmap.end() ; ++i)
	    {
		std::string s(i->first);
		while(s.size() < 20)
		    s.append(" ");
		stream << s;
		(i->second)->dump(stream);
		stream << std::endl;
	    }
	    stream << "*************************************************" << std::endl;
            stream << std::endl;
	}

	/* real public interface */
#	define optset(key,type,value) options_T::set<type>(key, value)
#	define optget(key,type)   options_T::get<type>(key)

};

#endif

/* vim: set tw=80 sw=4 et : */
