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
        static std::string _outfile;
        static bool _debug;
        static bool _timer;
        static bool _verbose;
        static bool _quiet;
        static bool _all;
        static bool _fetch;
        static std::string::size_type _maxcol;
        static options_action_T _action;
        static std::ostream *_outstream;

    public:
        void set_debug(bool value) { _debug = value; }
        bool debug() const { return _debug; }

        void set_timer(bool value) { _timer = value; }
        bool timer() const { return _timer; }

        void set_verbose(bool value) { _verbose = value; }
        bool verbose() const { return _verbose; }

        void set_quiet(bool value) { _quiet = value; }
        bool quiet() const { return _quiet; }

        void set_all(bool value) { _all = value; }
        bool all() const { return _all; }

        void set_fetch(bool value) { _fetch = value; }
        bool fetch() const { return _fetch; }

        void set_maxcol(std::string::size_type value) { _maxcol = value - 2; }
        std::string::size_type maxcol() const { return _maxcol; }

        void set_action(options_action_T value) { _action = value; }
        options_action_T action() const { return _action; }

        void set_herds_xml(const std::string &value) { _herds_xml = value; }
        const std::string &herds_xml() const { return _herds_xml; }

        void set_portdir(const std::string &value) { _portdir = value; }
        const std::string &portdir() const { return _portdir; }

        void set_outfile(const std::string &value) { _outfile = value; }
        const std::string &outfile() const { return _outfile; }

        void set_outstream(std::ostream *stream) { _outstream = stream; }
        std::ostream *outstream() const { return _outstream; }
};

#endif

/* vim: set tw=80 sw=4 et : */
