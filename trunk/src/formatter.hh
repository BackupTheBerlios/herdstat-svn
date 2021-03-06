/*
 * herdstat -- src/formatter.hh
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

#ifndef _HAVE_FORMAT_HH
#define _HAVE_FORMAT_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <iostream>
#include <string>
#include <vector>
#include <utility>

#include <herdstat/util/misc.hh>
#include <herdstat/util/regex.hh>

#include "query_results.hh"

class FormatAttrs
{
    public:
        std::string::size_type maxlen() const { return _maxlen; }
        void set_maxlen(std::string::size_type n) { _maxlen = n; }
        std::string::size_type maxlabel() const { return _maxlabel; }
        void set_maxlabel(std::string::size_type n) { _maxlabel = n; }

        const std::vector<std::string>& devaway() const { return _devaway; }
        void set_devaway(const std::vector<std::string>& v) { _devaway = v; }

        const herdstat::util::RegexMap<std::string>& highlights() const
        { return _highlights; }
        void add_highlights(const std::vector<std::string>& v);
        void add_highlight(const std::string &s,
                           const std::string &c = "")
        {
            _highlights.insert(std::make_pair(herdstat::util::Regex(s),
                (_colors? (c.empty() ? _hcolor : c) : "")));
        }

        const std::string& no_color() const { return _no_color; }

        const std::string& label_color() const { return _lcolor; }
        void set_label_color(const std::string& c)
        { _lcolor_save = _lcolor ; _lcolor = c; }

        const std::string& highlight_color() const { return _hcolor; }
        void set_highlight_color(const std::string& c)
        { _hcolor_save = _hcolor ; _hcolor  = c; }

        const std::string& devaway_color() const { return _dcolor; }
        void set_devaway_color(const std::string& c)
        { _dcolor_save = _dcolor ; _dcolor = c; }

        bool colors() const { return _colors; }
        void set_colors(bool c);

        bool quiet() const { return _quiet; }
        const std::string& quiet_delim() const { return _quiet_delim; }
        void set_quiet(bool quiet, std::string delim = "\n")
        { _quiet = quiet; _quiet_delim = delim; set_colors(not quiet); }

        bool marked_away() const { return _away; }
        void set_marked_away(bool a) { _away = a; }

    private:
        friend class Formatter;
        FormatAttrs();
        FormatAttrs(const FormatAttrs&);
        FormatAttrs& operator= (const FormatAttrs&);

        herdstat::util::ColorMap& _cmap;
        bool _quiet;
        bool _colors;
        bool _away;
        std::string _quiet_delim;
        std::string::size_type _maxlen;
        std::string::size_type _maxlabel;
        std::string _lcolor, _lcolor_save; /* label color */
        std::string _hcolor, _hcolor_save; /* highlight color */
        std::string _dcolor, _dcolor_save; /* devaway color */
        std::string _no_color, _no_color_save;
        std::vector<std::string> _devaway;
        herdstat::util::RegexMap<std::string> _highlights;
};

class Formatter
{
    public:
        /// Flush results buffer to stream.
        void operator()(const QueryResults& results, std::ostream& stream);

        /// Get attributes.
        FormatAttrs& attrs() { return _attrs; }

    private:
        friend Formatter& GlobalFormatter();
        Formatter() { }

        FormatAttrs _attrs;
};

/**
 * Single point of access for the Formatter class.
 * @returns Reference to a static local Formatter instance.
 */
inline Formatter&
GlobalFormatter()
{
    static Formatter f;
    return f;
}

#endif /* _HAVE_FORMAT_HH */

/* vim: set tw=80 sw=4 fdm=marker et : */
