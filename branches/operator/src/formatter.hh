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
#include <utility>

#include "util.hh"

class formatter_T
{
    private:
        /* format attributes */
        class attrs_T
        {
            public:
                attrs_T();

                std::ostream *stream;

                bool colors;
                bool quiet;

                std::string label_color;
                std::string data_color;
                std::string highlight_color;

                std::string::size_type maxtotal;
                std::string::size_type maxlabel;
                std::string::size_type maxdata;
            
                std::string::size_type maxctotal;
                std::string::size_type maxclabel;
                std::string::size_type maxcdata;

                std::vector<std::string> highlights;
        };

        class buf_T
            : public std::vector<std::pair<std::string, std::string> >
        {
            public:
                void operator() (std::string l, std::string d)
                {
                    push_back(std::make_pair(l, d));
                }

                void operator() (std::string l, std::vector<std::string> d)
                {
                    std::string s;
                    std::vector<std::string>::iterator i;
                    for (i = d.begin() ; i != d.end() ; ++i)
                        s.append(*i + " ");
                    s.erase(s.length() - 1);
                    push_back(std::make_pair(l, s));
                }
        };

        std::string highlight(std::vector<std::string>);

        static attrs_T attrs;
        static buf_T buf;
        util::color_map_T color;

    public:
        typedef std::size_t size_type;

        formatter_T(std::ostream *s) { attrs.stream = s; }
        ~formatter_T() { flush(); }

        void operator() (std::string l, std::string d) { return buf(l, d); }
        void operator() (std::string l, std::vector<std::string> d)
        {
            if (quiet())
            {
                std::vector<std::string>::iterator i;
                for (i = d.begin() ; i != d.end() ; ++i)
                    buf("", *i);
            }
            else
                buf(l, d);
        }

        void flush();
        void endl() { buf("" ,"\n"); }
        const std::string &peek() const { return buf.back().second; }
        const buf_T::size_type size() const { return buf.size(); }
        void set_attrs();

        /* attribute member functions */
        void set_ostream(std::ostream *s) { attrs.stream = s; }
        std::ostream *ostream() { return attrs.stream; }

        void set_maxtotal(size_type s) { attrs.maxtotal = s; }
        size_type maxtotal() { return attrs.maxtotal; }
            
        void set_maxlabel(size_type s) { attrs.maxlabel = s; }
        size_type maxlabel() { return attrs.maxlabel; }

        void set_maxdata(size_type s) { attrs.maxdata = s; }
        size_type maxdata() { return attrs.maxdata; }

        void set_colors(bool value) { attrs.colors = value; }
        bool colors() { return attrs.colors; }

        void set_quiet(bool value) { attrs.quiet = value; }
        bool quiet() { return attrs.quiet; }

        void set_labelcolor(std::string &s) { attrs.label_color = s; }
        std::string &labelcolor() { return attrs.label_color; }

        void set_datacolor(std::string &s) { attrs.data_color = s; }
        std::string &datacolor() { return attrs.data_color; }

        void set_highlightcolor(std::string &s) { attrs.highlight_color = s; }
        std::string &highlightcolor() { return attrs.highlight_color; }

        void add_highlight(std::string s) { attrs.highlights.push_back(s); }
};

#endif

/* vim: set tw=80 sw=4 et : */
