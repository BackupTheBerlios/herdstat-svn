/*
 * herdstat -- src/formatter.cc
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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string>
#include <vector>
#include <algorithm>
#include <iterator>

#include "util.hh"
#include "formatter.hh"
#include "exceptions.hh"

format_attrs_T formatter_T::attr;
std::vector<std::string> formatter_T::buffer;

/*
 * Default constructor
 * Set sane default attributes
 */

format_attrs_T::format_attrs_T()
{
    colors = false;
    quiet = false;
    maxtotal = 78;
    maxlabel = 20;
    maxdata  = 58;
}

/*
 * Finalize the attributes ; calculate increase in string length
 * because of the colors, etc. This is called once by the caller
 * after the attrs have been set.
 */

void
formatter_T::set_attrs()
{
    util::color_map_T color;

    if (quiet())
        attr.colors = false;
    else
        /* +1 for ':' */
        attr.maxlabel++;

    /* -2 just for padding */
    attr.maxdata -= 2;
    attr.maxtotal = attr.maxlabel + attr.maxdata;

    /* if we're using colors, we need to compensate
     * for the increase in string length.           */
    if (attr.colors)
    {
        attr.maxclabel = attr.maxlabel
                       + attr.label_color.length()
                       + color[none].length();
        attr.maxcdata  = attr.maxdata
                       + attr.data_color.length()
                       + color[none].length();
        attr.maxctotal = attr.maxclabel + attr.maxcdata;
    }
    else
    {
        attr.label_color = attr.data_color = "";
        attr.maxclabel = attr.maxlabel;
        attr.maxcdata  = attr.maxdata;
        attr.maxctotal = attr.maxtotal;
    }
}

/*
 * Overloaded append() that takes a vector
 */

void
formatter_T::append(const std::string &label, std::vector<std::string> data)
{
    std::vector<std::string>::iterator i;

    /* if quiet, handle it here, as we're going to end up splitting
     * the data string into a vector anyways */
    if (quiet())
        std::copy(data.begin(), data.end(), std::back_inserter(buffer));

    /* otherwise, produce a data string and call the real append() */
    else
    {
        std::string s;
        for (i = data.begin() ; i != data.end() ; ++i)
            s += *i + " ";
    
        if (s[s.length() - 1] == ' ')
            s.erase(s.length() - 1);

        append(label, s);
    }
}

/*
 * Append text to the output buffer.  Smartly handle lines that are bigger
 * than maxcol (new line and indent), and make sure it all aligns nicely.
 */

void
formatter_T::append(const std::string &label, const std::string &data)
{
    util::color_map_T color;
    std::string cur;

    /* don't display the label (or indent) if the quiet attr is set */
    if (not quiet())
    {
        if (label.length() > attr.maxlabel)
            throw format_E("Label '%s' is greater than maxlabel attribute (%d)",
                label.c_str(), attr.maxlabel);

        cur = color[green] + label + color[none] +
            (label.empty() ? "" : ":");

        while (cur.length() < attr.maxclabel)
            cur.append(" ");
    }

    if (not data.empty())
    {
        if (quiet())
            append(label, util::splitstr(data));
        else
        {
            cur.append(attr.data_color);

            /* will it all fit on one line? */
            if ((cur.length() + data.length() +
                color[none].length()) < attr.maxctotal)
                cur.append(data);
            else
            {
                /* line's full, so find a location where we can truncate */
                std::string::size_type pos = data.rfind(" ", attr.maxdata);
                cur.append(pos == std::string::npos ? data : data.substr(0, pos));
                buffer.push_back(cur);
                cur.clear();
            
                /* indent */
                while (cur.length() < attr.maxlabel)
                    cur.append(" ");

                /* handle leftovers */
                std::vector<std::string> leftovers = util::splitstr(data.substr(pos));

                std::vector<std::string>::iterator i;
                for (i = leftovers.begin() ; i != leftovers.end() ; ++i)
                {
                    /* does it fit on the current line? */
                    if ((cur.length() + (*i).length()) > attr.maxtotal)
                    {
                        buffer.push_back(cur);
                        cur.clear();

                        /* indent */
                        while (cur.length() < attr.maxlabel)
                            cur.append(" ");
                    }
            
                    cur += *i + " ";
                }
            }
        }
    }

    if (cur.length() > 0)
        buffer.push_back(cur);
}

/*
 * Flush our buffer's contents to the specified stream,
 * removing each element in the buffer as we do so.
 */

void
formatter_T::flush(std::ostream &stream)
{
    std::remove_copy(buffer.begin(), buffer.end(),
        std::ostream_iterator<std::string>(stream, "\n"),
        "supercalifragilisticexpialidocious");
}

/* vim: set tw=80 sw=4 et : */
