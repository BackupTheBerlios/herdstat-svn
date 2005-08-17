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

#ifndef HAVE_FORMATTER_HH
#define HAVE_FORMATTER_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <ostream>
#include <map>

#include "common.hh"

class formatter_T
{
    public:
        typedef std::size_t size_type;
        typedef util::string string_type;
        typedef std::vector<string_type> buffer_type;
        typedef util::color_map_T color_type;

        formatter_T() { }

        void operator() (const string_type &l, const string_type &d)
        { append(l, d); }

        void operator() (const string_type &l, const std::vector<string_type> &d)
        { append(l, d); }

        void endl() { buffer.push_back(""); }
        void flush(std::ostream &);
        const string_type &peek() const { return buffer.back(); }
        buffer_type::size_type size() const { return buffer.size(); }

        void set_attrs();

        /* attribute member functions */
        void set_marked_away(const bool &value) { attr.marked_away = value; }
        const bool &marked_away() const { return attr.marked_away; }

        void set_maxtotal(const size_type &s) { attr.maxtotal = s; }
        const size_type &maxtotal() const { return attr.maxtotal; }
            
        void set_maxlabel(const size_type &s) { attr.maxlabel = s; }
        const size_type &maxlabel() const { return attr.maxlabel; }

        void set_maxdata(const size_type &s) { attr.maxdata = s; }
        const size_type &maxdata() const { return attr.maxdata; }

        void set_colors(const bool &value) { attr.colors = value; }
        const bool &colors() const { return attr.colors; }

        void set_quiet(const bool &value, const string_type &delim = "\n")
        {
            attr.quiet = value;
            attr.quiet_delim = delim;
        }
        const bool &quiet() const { return attr.quiet; }

        void set_labelcolor(const string_type &s) { attr.label_color = s; }
        const string_type &labelcolor() const { return attr.label_color; }

        void set_datacolor(const string_type &s) { attr.data_color = s; }
        const string_type &datacolor() const { return attr.data_color; }

        void set_highlightcolor(const string_type &s) { attr.highlight_color = s; }
        const string_type &highlightcolor() const { return attr.highlight_color; }

        void set_devaway_color(const string_type &s) { attr.devaway_color = s; }
        const string_type &devaway_color() const { return attr.devaway_color; }

        void add_highlight(const string_type &s,
                           const string_type &c = attr.highlight_color)
        { attr.highlights[s] = attr.colors? c : ""; }
        void add_highlights(const std::vector<string_type> &);

        void set_devaway(const std::vector<string_type> &v)
        { attr.devaway = v; }

    private:
        /* format attributes */
        struct attrs_type
        {
            attrs_type();

            bool colors;
            bool quiet;
            bool marked_away;

            string_type quiet_delim;
            string_type label_color;
            string_type data_color;
            string_type highlight_color;
            string_type devaway_color;
            string_type no_color;

            string_type::size_type maxtotal;
            string_type::size_type maxlabel;
            string_type::size_type maxdata;
            
            string_type::size_type maxctotal;
            string_type::size_type maxclabel;
            string_type::size_type maxcdata;

            std::map<string_type, string_type> highlights;
            std::vector<string_type> devaway;
        };

        string_type highlight(const std::vector<string_type> &);
        void append(const string_type &, const string_type &);
        void append(const string_type &, const std::vector<string_type> &);

        static buffer_type buffer;
        static attrs_type attr;
        static color_type color;
};

#endif

/* vim: set tw=80 sw=4 et : */
