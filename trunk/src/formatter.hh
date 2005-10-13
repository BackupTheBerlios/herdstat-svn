/*
 * herdstat -- src/format.hh
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

struct FirstLengthLess
{
    bool operator()(const std::pair<std::string, std::string>& p1,
                    const std::pair<std::string, std::string>& p2) const
    {
        return (p1.first.length() < p2.first.length());
    }
};

class FormatAttrs
{
    public:
        FormatAttrs();

        std::size_t maxlen() const { return _maxlen; }
        void set_maxlen(std::size_t n) { _maxlen = n; }

        const std::vector<std::string>& devaway() const { return _devaway; }
        void set_devaway(const std::vector<std::string>& v) { _devaway = v; }

        const std::map<std::string, std::string>& highlights() const
        { return _highlights; }
        void add_highlights(const std::vector<std::string>& v);
        void add_highlight(const std::string &s,
                           const std::string &c = "")
        { _highlights[s] = (_colors? (c.empty() ? _hcolor : c) : ""); }

        const std::string& no_color() const { return _no_color; }

        const std::string& label_color() const { return _lcolor; }
        void set_label_color(const std::string& c) { _lcolor = c; }

        const std::string& highlight_color() const { return _hcolor; }
        void set_highlight_color(const std::string& c) { _hcolor = c; }

        const std::string& devaway_color() const { return _dcolor; }
        void set_devaway_color(const std::string& c) { _dcolor = c; }

        bool colors() const { return _colors; }
        void set_colors(bool c)
        {
            _colors = c;
            if (not _colors)
            {
                _hcolor.clear(); _dcolor.clear();
                _lcolor.clear();
            }
        }

        bool quiet() const { return _quiet; }
        const std::string& quiet_delim() const { return _quiet_delim; }
        void set_quiet(bool quiet, std::string delim = "\n")
        { _quiet = quiet; _quiet_delim = delim; set_colors(not quiet); }

        bool marked_away() const { return _away; }
        void set_marked_away(bool a) { _away = a; }

    private:
        herdstat::util::ColorMap _cmap;
        bool _quiet;
        bool _colors;
        bool _away;
        std::string _quiet_delim;
        std::size_t _maxlen;
        std::string _lcolor; /* label color */
        std::string _hcolor; /* highlight color */
        std::string _dcolor; /* devaway color */
        std::string _no_color;
        std::vector<std::string> _devaway;
        std::map<std::string, std::string> _highlights;
};

class Formatter
{
    public:
        typedef std::vector<std::pair<std::string, std::string> > buffer_type;

        /// Get attributes.
        FormatAttrs& attrs() { return _attrs; }
        /// Set attributes.
        void set_attrs(const FormatAttrs& attrs) { _attrs = attrs; }

        /** Append label/data.
         * @param label Label describing data
         * @param data data string
         */
        void operator()(const std::string& label, const std::string& data)
        {
            this->append((not _attrs.quiet() and not label.empty()) ?
                        label : "", data);
        }

        /** Append label/data.
         * @param label Label describing data
         * @param data data vector
         */
        void operator()(const std::string& label,
                        const std::vector<std::string>& data);

        /// Append new line.
        void endl() { this->append("", ""); }

        /// Flush buffer to stream.
        void flush(std::ostream& stream);

        /// Clear buffer.
        void clear() { _buffer.clear(); }

        /// Get buffer size.
        buffer_type::size_type size() { return _buffer.size(); }
        /// Get reference to last element of buffer.
        buffer_type::reference peek() { return _buffer.back(); }

    private:
        friend Formatter& GlobalFormatter();

        Formatter();
        Formatter(const FormatAttrs& attrs);

        /// Raw access to the buffer.
        void append(const std::string& label, const std::string& data)
        { _buffer.push_back(buffer_type::value_type(label, data)); }
        //// Perform highlighting
        void highlight(std::vector<std::string>& v);

        FormatAttrs _attrs;
        buffer_type _buffer;
};

inline Formatter&
GlobalFormatter()
{
    static Formatter f;
    return f;
}

#endif /* _HAVE_FORMAT_HH */

/* vim: set tw=80 sw=4 et : */
