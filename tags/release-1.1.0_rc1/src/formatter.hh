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
#include <string>
#include <vector>
#include <map>

class formatter_T
{
    private:
        /* format attributes */
        struct attrs_T
        {
            attrs_T();

            bool colors;
            bool quiet;

            std::string quiet_delim;

            std::string label_color;
            std::string data_color;
            std::string highlight_color;
            std::string no_color;

            std::string::size_type maxtotal;
            std::string::size_type maxlabel;
            std::string::size_type maxdata;
            
            std::string::size_type maxctotal;
            std::string::size_type maxclabel;
            std::string::size_type maxcdata;

            std::vector<std::string> highlights;
        };

        std::string highlight(std::vector<std::string>);
        void append(const std::string &, const util::string &);
        void append(const std::string &, std::vector<std::string>);

        static std::vector<std::string> buffer;
        static attrs_T attr;
        util::color_map_T color;

    public:
        typedef std::size_t size_type;

        formatter_T() { }

        void operator() (const std::string &l, const util::string &d)
        {
            append(l, d);
        }

        void operator() (const std::string &l, std::vector<std::string> d)
        {
            append(l, d);
        }

        void endl() { buffer.push_back(""); }
        void flush(std::ostream &);
        const std::string &peek() const { return buffer.back(); }
        std::vector<std::string>::size_type size() const { return buffer.size(); }

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

        void set_quiet(bool value, const std::string delim = "\n")
        {
            attr.quiet = value;
            attr.quiet_delim = delim;
        }
        bool quiet() { return attr.quiet; }

        void set_labelcolor(std::string &s) { attr.label_color = s; }
        std::string &labelcolor() { return attr.label_color; }

        void set_datacolor(std::string &s) { attr.data_color = s; }
        std::string &datacolor() { return attr.data_color; }

        void set_highlightcolor(std::string &s) { attr.highlight_color = s; }
        std::string &highlightcolor() { return attr.highlight_color; }

        void add_highlight(std::string s) { attr.highlights.push_back(s); }
};

#endif

/* vim: set tw=80 sw=4 et : */
