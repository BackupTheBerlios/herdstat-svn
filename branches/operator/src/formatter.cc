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
#include <utility>

#include "util.hh"
#include "formatter.hh"
#include "exceptions.hh"

formatter_T::buf_T formatter_T::buf;
formatter_T::attrs_T formatter_T::attrs;

/*
 * Default constructor
 * Set sane default attributes
 */

formatter_T::attrs_T::attrs_T()
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
    if (quiet())
        attrs.colors = false;
    else
        /* +1 for ':' */
        attrs.maxlabel++;

    /* -2 just for padding */
    attrs.maxdata -= 2;
    attrs.maxtotal = attrs.maxlabel + attrs.maxdata;

    /* if we're using colors, we need to compensate
     * for the increase in string length.           */
    if (attrs.colors)
    {
        attrs.maxclabel = attrs.maxlabel
                       + attrs.label_color.length()
                       + color[none].length();
        attrs.maxcdata  = attrs.maxdata;
        attrs.maxctotal = attrs.maxclabel + attrs.maxcdata;
    }
    else
    {
        attrs.label_color = attrs.data_color = "";
        attrs.maxclabel = attrs.maxlabel;
        attrs.maxcdata  = attrs.maxdata;
        attrs.maxctotal = attrs.maxtotal;
    }
}

/*
 * Given a vector of data, see if any of the elements
 * match words that should be highlighted.  Return the
 * string, highlighted or not.
 */

std::string
formatter_T::highlight(std::vector<std::string> data)
{
    std::string s;
    std::vector<std::string>::iterator i;

    for (i = data.begin() ; i != data.end() ; ++i)
    {
        if (std::find(attrs.highlights.begin(),
            attrs.highlights.end(), *i) != attrs.highlights.end())
            s += attrs.highlight_color + (*i) + color[none] + " ";
        else
            s += *i + " ";
    }

    return s;
}

/*
 * Overloaded append() that takes a vector
 */

//void
//formatter_T::append(const std::string &label, std::vector<std::string> data)
//{
//    std::vector<std::string>::iterator i;

    /* if quiet, handle it here, as we're going to end up splitting
     * the data string into a vector anyways */
//    if (quiet())
//        std::copy(data.begin(), data.end(), std::back_inserter(buffer));

    /* otherwise, produce a data string and call the real append() */
//    else
//    {
//        std::string s;
//        for (i = data.begin() ; i != data.end() ; ++i)
//            s += *i + " ";
//    
//        if (s[s.length() - 1] == ' ')
//            s.erase(s.length() - 1);

//        append(label, s);
//    }
//}

/*
 * Flush buffer, formatting it on its way out.
 */

void
formatter_T::flush()
{
    buf_T::iterator b;
    for (b = buf.begin() ; b != buf.end() ; ++b)
    {
        std::string label(b->first);
        std::string data(b->second);
        std::string cur;

        /* don't display the label (or indent) if the quiet attr is set */
        if (not quiet())
        {
            if (label.length() > attrs.maxlabel)
                throw format_E("Label '%s' is greater than maxlabel attribute (%d)",
                    label.c_str(), attrs.maxlabel);

            /* TODO: fix me. */
            cur = color[green] + label + color[none] +
                (label.empty() ? "" : ":");

            while (cur.length() < attrs.maxclabel)
                cur.append(" ");
        }

        if (not data.empty())
        {
            if (quiet())
                buf(label, util::splitstr(data));
            else
            {
                if ((cur.length() + data.length()) < attrs.maxctotal)
                    cur += highlight(util::splitstr(data));
                else
                {
                    /* line's full, so find a location where we can truncate */
                    std::string::size_type pos = data.rfind(" ", attrs.maxdata);
                    cur += highlight(util::splitstr(
                        (pos == std::string::npos ? data : data.substr(0, pos))));

                    *(attrs.stream) << cur << std::endl;
                    cur.clear();

                    /* indent */
                    while (cur.length() < attrs.maxlabel)
                        cur.append(" ");

                    /* handle leftovers */
                    std::vector<std::string> leftovers = util::splitstr(data.substr(pos));

                    std::vector<std::string>::iterator i;
                    for (i = leftovers.begin() ; i != leftovers.end() ; ++i)
                    {
                        std::string::size_type oldlen = 0;
                        bool highlight_found = false;

                        /* should the current word be highlighted? */
                        if (std::find(attrs.highlights.begin(),
                            attrs.highlights.end(), *i) != attrs.highlights.end())
                        {
                            highlight_found = true;

                            /* adjust maxtotal appropriately */
                            oldlen = attrs.maxtotal;
                            attrs.maxtotal = oldlen + attrs.highlight_color.length() +
                                color[none].length();
                        }

                        std::string::size_type curlen;

                        /*
                         * Does cur contain a previously highlighted word? If so,
                         * we need to compensate for the color lengths.
                         */
                        if ((cur.find("\033") != std::string::npos))
                            curlen = cur.length() - attrs.highlight_color.length() -
                                color[none].length();

                        /* compensate for current highlight? */
                        else if (highlight_found)
                            curlen = cur.length() + attrs.highlight_color.length() +
                                color[none].length();

                        /* don't compensate */
                        else
                            curlen = cur.length();

                        /* does it fit on the current line? */
                        if ((curlen + (*i).length()) > attrs.maxtotal)
                        {
                            *(attrs.stream) << cur << std::endl;
                            cur.clear();

                            /* indent */
                            while (cur.length() < attrs.maxlabel)
                                cur.append(" ");
                        }
            
                        if (highlight_found)
                        {
                            cur += attrs.highlight_color + (*i) + color[none] + " ";

                            /* restore saved maxtotal */
                            attrs.maxtotal = oldlen;
                        }   
                        else
                            cur += *i + " ";
                    }
                }
            }
        }

        if (cur.size() > 0)
            *(attrs.stream) << cur << std::endl;
    }


    buf.clear();
}

/* vim: set tw=80 sw=4 et : */
