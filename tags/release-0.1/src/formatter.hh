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

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <exception>
#include <cstdio>
#include <cstdarg>

#include "util.hh"

namespace fm
{
    /* format attributes */
    class format_attrs_T
    {
        private:
            util::color_map_T colormap;

        public:
            format_attrs_T();

            std::string::size_type maxtotal;
            std::string::size_type maxlabel;
            std::string::size_type maxdata;

            std::string::size_type maxctotal;
            std::string::size_type maxclabel;
            std::string::size_type maxcdata;

            bool colors;
            std::string label_color;
            std::string data_color;

            const std::string &color(color_name_T c) { return colormap[c]; }
    };

    class formatter_T
    {
        private:
            std::vector<std::string> buffer;

        public:
            typedef int size_type;

            formatter_T() { }
            formatter_T(format_attrs_T &fmattr)
                : attr(fmattr) { set_attrs(); }

            void endl() { buffer.push_back(""); }
            void append(const std::string &, const std::string &);
            void append(const std::string &, std::vector<std::string> &);
            void flush(std::ostream &);

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

            void set_labelcolor(std::string &s) { attr.label_color = s; }
            std::string &labelcolor() { return attr.label_color; }

            void set_datacolor(std::string &s) { attr.data_color = s; }
            std::string &datacolor() { return attr.data_color; }

            format_attrs_T attr;
    };
}

#endif

/* vim: set tw=80 sw=4 et : */
