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

#include <string>

enum options_action_T
{
    action_unspecified,
    action_herd,
    action_dev,
    action_pkg
};

class options_T
{
    protected:
        static std::string _herds_xml;
        static std::string _portdir;
        static bool _debug;
        static bool _timer;
        static bool _verbose;
        static bool _quiet;
        static std::string::size_type _maxcol;
        static options_action_T _action;

    public:
        void set_debug(bool value) { options_T::_debug = value; }
        bool debug() { return options_T::_debug; }

        void set_timer(bool value) { options_T::_timer = value; }
        bool timer() { return options_T::_timer; }

        void set_verbose(bool value) { options_T::_verbose = value; }
        bool verbose() { return options_T::_verbose; }

        void set_quiet(bool value) { options_T::_quiet = value; }
        bool quiet() { return options_T::_quiet; }

        void set_maxcol(std::string::size_type value) { options_T::_maxcol = value - 2; }
        std::string::size_type maxcol() { return options_T::_maxcol; }

        void set_action(options_action_T value) { options_T::_action = value; }
        options_action_T action() { return options_T::_action; }

        void set_herds_xml(std::string const &value) { options_T::_herds_xml = value; }
        std::string const &herds_xml() { return options_T::_herds_xml; }

        void set_portdir(std::string const &value) { options_T::_portdir = value; }
        std::string const &portdir() { return options_T::_portdir; }
};

#endif

/* vim: set tw=80 sw=4 et : */
