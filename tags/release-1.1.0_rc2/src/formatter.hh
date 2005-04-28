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
        typedef std::vector<util::string> buffer_type;
        typedef util::color_map_T color_type;

        formatter_T() { }

        void operator() (const util::string &l, const util::string &d)
        { append(l, d); }

        void operator() (const util::string &l, const std::vector<util::string> &d)
        { append(l, d); }

        void endl() { buffer.push_back(""); }
        void flush(std::ostream &);
        const util::string &peek() const { return buffer.back(); }
        std::vector<util::string>::size_type size() const { return buffer.size(); }

        void set_attrs();

        /* attribute member functions */
        void set_maxtotal(size_type s) { attr.maxtotal = s; }
        size_type maxtotal() { return attr.maxtotal; }
            
        void set_maxlabel(size_type s) { attr.maxlabel = s; }
        size_type maxlabel() { return attr.maxlabel; }

        void set_maxdata(size_type s) { attr.maxdata = s; }
        size_type maxdata() { return attr.maxdata; }

        void set_colors(bool value) { attr.colors = value; }
        bool colors() { return attr.colors; }

        void set_quiet(bool value, const util::string delim = "\n")
        {
            attr.quiet = value;
            attr.quiet_delim = delim;
        }
        bool quiet() { return attr.quiet; }

        void set_labelcolor(util::string &s) { attr.label_color = s; }
        util::string &labelcolor() { return attr.label_color; }

        void set_datacolor(util::string &s) { attr.data_color = s; }
        util::string &datacolor() { return attr.data_color; }

        void set_highlightcolor(util::string &s) { attr.highlight_color = s; }
        util::string &highlightcolor() { return attr.highlight_color; }

        void add_highlight(util::string s) { attr.highlights.push_back(s); }

    private:
        /* format attributes */
        struct attrs_type
        {
            attrs_type();

            bool colors;
            bool quiet;

            util::string quiet_delim;
            util::string label_color;
            util::string data_color;
            util::string highlight_color;
            util::string no_color;

            util::string::size_type maxtotal;
            util::string::size_type maxlabel;
            util::string::size_type maxdata;
            
            util::string::size_type maxctotal;
            util::string::size_type maxclabel;
            util::string::size_type maxcdata;

            std::vector<util::string> highlights;
        };

        util::string highlight(const std::vector<util::string> &);
        void append(const util::string &, const util::string &);
        void append(const util::string &, const std::vector<util::string> &);

        static buffer_type buffer;
        static attrs_type attr;
};

#endif

/* vim: set tw=80 sw=4 et : */
