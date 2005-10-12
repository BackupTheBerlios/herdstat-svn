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

#include <algorithm>
#include <functional>
#include <iterator>

#include <herdstat/exceptions.hh>
#include <herdstat/util/string.hh>
#include <herdstat/util/regex.hh>
#include "formatter.hh"

using namespace herdstat;

/** static members **********************************************************/
formatter_T::attrs_type  formatter_T::attr;
formatter_T::buffer_type formatter_T::buffer;
formatter_T::color_type formatter_T::color;
/****************************************************************************/
formatter_T::attrs_type::attrs_type()
{
    colors = quiet = marked_away = false;
    maxtotal = 78;
    maxlabel = 20;
    maxdata  = 58;
}
/****************************************************************************
 * Finalize the attributes ; calculate increase in string length            *
 * because of the colors, etc. This is called once by the caller            *
 * after the attrs have been set.                                           *
 ****************************************************************************/
void
formatter_T::set_attrs()
{
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
        attr.no_color = color[none];
        attr.maxclabel = attr.maxlabel
                       + attr.label_color.length()
                       + attr.no_color.length();
        attr.maxcdata  = attr.maxdata;
        attr.maxctotal = attr.maxclabel + attr.maxcdata;
    }
    else
    {
        attr.label_color.clear();
        attr.data_color.clear();
        attr.highlight_color.clear();
        attr.devaway_color.clear();
        attr.maxclabel = attr.maxlabel;
        attr.maxcdata  = attr.maxdata;
        attr.maxctotal = attr.maxtotal;
    }
}
/****************************************************************************
 * Given a vector of strings in the format of 'word,color', split each      *
 * element and insert them into the highlights map.                         *
 ****************************************************************************/
void
formatter_T::add_highlights(const std::vector<string_type> &pairs)
{
    std::vector<string_type>::const_iterator i, e = pairs.end();
    for (i = pairs.begin() ; i != e ; ++i)
    {
        std::vector<string_type> parts(util::split(*i, ','));
        if (parts.size() == 1)
        {
            std::string c(attr.colors? attr.highlight_color : "");
            attr.highlights[parts.front()] = c;
        }
        else if (parts.size() == 2)
        {
            std::string c(attr.colors? color[parts.back()] : "");
            attr.highlights[parts.front()] = c;
        }
        else
            throw Exception("Invalid highlight '%s'", i->c_str());
    }
}
/****************************************************************************
 * Given a vector of data, see if any of the elements                       *
 * match words that should be highlighted.  Return the                      *
 * string, highlighted or not.                                              *
 ****************************************************************************/
formatter_T::string_type
formatter_T::highlight(const std::vector<string_type> &data)
{
    string_type s;
    std::vector<string_type>::const_iterator i, e = data.end();

    for (i = data.begin() ; i != e ; ++i)
    {
        bool highlight = false;
        /* search highlights */
        std::map<string_type, string_type>::iterator h,
            he = attr.highlights.end();
        for (h = attr.highlights.begin() ; h != he ; ++h)
        {
            string_type::size_type pos = h->first.find("re:");
            if (pos == string_type::npos and h->first == *i)
            {
                highlight = true;
                if (s.find((*i)+attr.no_color) == string_type::npos)
                    s += h->second + (*i) + attr.no_color +  " ";
            }
            else if (pos != string_type::npos)
            {
                util::Regex regex(h->first.substr(pos+3));
                if (regex == *i)
                {
                    highlight = true;
                    if (s.find((*i)+attr.no_color) == string_type::npos)
                        s += h->second + (*i) + attr.no_color + " ";
                }
            }
        }

        if (not highlight)
        {
            string_type tmp(*i);

            /* loop removing any ANSI colors, so we can search
             * the devway vector */
            if (tmp.find("\033") != string_type::npos)
            {
                string_type::size_type pos, lpos = 0;
                while (true)
                {
                    if ((pos = tmp.find("\033", lpos)) == string_type::npos)
                        break;

                    string_type::size_type mpos;
                    if ((mpos = tmp.find('m', pos)) == string_type::npos)
                        break;

                    tmp.erase(pos, mpos+1);

                    lpos = ++pos;
                }
            }
                    
            /* search devaway */
            if (std::find(attr.devaway.begin(),
                attr.devaway.end(), tmp) != attr.devaway.end())
            {
//                debug_msg("marking '%s' as away", i->c_str());
                attr.marked_away = true;
                s += (*i) + attr.devaway_color + "*" + attr.no_color + " ";
            }
            else
                s += (*i) + " ";
        }
    }

    return s;
}
/****************************************************************************/
void
formatter_T::append(const string_type &label,
                    const std::vector<string_type> &data)
{
    /* if quiet, handle it here, as we're going to end up splitting
     * the data string into a vector anyways */
    if (attr.quiet and attr.quiet_delim == " ")
        buffer.push_back(util::join(data));

    else if (attr.quiet)
        buffer.insert(buffer.end(), data.begin(), data.end());
//        std::copy(data.begin(), data.end(), std::back_inserter(buffer));

    /* otherwise, produce a data string and call the real append() */
    else
        append(label, util::join(data));
}
/****************************************************************************
 * Append text to the output buffer.  Smartly handle lines that are bigger  *
 * than maxcol (new line and indent), and make sure it all aligns nicely.   *
 ****************************************************************************/
void
formatter_T::append(const string_type &label, const string_type &data)
{
    string_type cur;

    /* don't display the label (or indent) if the quiet attr is set */
    if (not quiet())
    {
        if (label.length() > attr.maxlabel)
            throw Exception("Label '%s' is greater than maxlabel attribute (%d)",
                label.c_str(), attr.maxlabel);

        if (not label.empty())
        {
            cur = attr.label_color + label + attr.no_color + ":";

            while (cur.length() < attr.maxclabel)
                cur.append(" ");
        }
        else
            while (cur.length() < attr.maxlabel)
                cur.append(" ");
    }

    if (not data.empty())
    {
//        debug_msg("data = '%s'", data.c_str());

        if (quiet())
            append(label, util::split(data));
        else
        {
            size_type maxlen =
                (cur.find("\033") == string_type::npos ?
                attr.maxtotal : attr.maxctotal);

            if ((cur.length() + data.length()) < maxlen)
            {
                cur += highlight(util::split(data));
//                debug_msg("it all fits on one line");
            }
            else
            {
                /* line's full, so find a location where we can truncate */
                string_type::size_type pos = data.rfind(" ", attr.maxdata-3);
                if (pos == string_type::npos)
                    cur += highlight(util::split(data));
                else
                    cur += highlight(util::split(data.substr(0, pos)));
//                debug_msg("pushing back '%s'", cur.c_str());
                buffer.push_back(cur);
                cur.clear();

                if (pos == string_type::npos)
                    return;

                /* indent */
                while (cur.length() < attr.maxlabel)
                    cur.append(" ");

//                debug_msg("handling leftovers '%s'", data.substr(pos).c_str());

                /* handle leftovers */
//                std::vector<string_type> leftovers = data.substr(pos).split();

                std::vector<string_type> leftovers =
                    util::split(highlight(util::split(data.substr(pos))));

                std::vector<string_type>::iterator i, e = leftovers.end();
                for (i = leftovers.begin() ; i != e ; ++i)
                {
                    string_type::size_type oldlen = 0;
                    bool highlight_found = false,
                         marked_away = false;

                    /* should the current word be highlighted? */
                    std::map<string_type, string_type>::iterator h;
                    for (h = attr.highlights.begin() ;
                         h != attr.highlights.end() ; ++h)
                    {
                        string_type::size_type pos = h->first.find("re:");
                        if (pos == string_type::npos and h->first == *i)
                        {
                            highlight_found = true;
                            oldlen = attr.maxtotal;
                            attr.maxtotal = oldlen + color[h->second].length() +
                                attr.no_color.length();
                        }
                        else if (pos != string_type::npos)
                        {
                            util::Regex regex(h->first.substr(pos+3));
                            if (regex == *i)
                            {
                                highlight_found = true;
                                oldlen = attr.maxtotal;
                                attr.maxtotal = oldlen + color[h->second].length()
                                    + attr.no_color.length();
                            }
                        }
                    }

                    if (std::find(attr.devaway.begin(),
                        attr.devaway.end(), *i) != attr.devaway.end())
                    {
                        attr.marked_away = true;
                        marked_away = true;
                        oldlen = attr.maxtotal;
                        attr.maxtotal = oldlen + attr.devaway_color.length() +
                            attr.no_color.length();
                    }

                    string_type::size_type curlen = cur.length();

                    /*
                     * Does cur contain a previously highlighted word? If so,
                     * we need to compensate for the color lengths.
                     */
                    if (cur.find("\033") != string_type::npos)
                    {
                        string_type::size_type lpos = 0;
                        while (true)
                        {
                            if ((pos = cur.find("\033", lpos)) == string_type::npos)
                                break;

                            curlen -= attr.highlight_color.length() -
                                attr.no_color.length();

                            lpos = ++pos;
                        }
                    }

                    /* compensate for current highlight? */
                    if (highlight_found)
                    {
                        curlen += attr.highlight_color.length() +
                            attr.no_color.length();

//                        debug_msg("highlighted '%s'; setting curlen to %d",
//                            i->c_str(), curlen);
                    }

                    if (marked_away)
                    {
                        curlen += attr.devaway_color.length() +
                            attr.no_color.length() + 1;
                    }

                    /* does it fit on the current line? */
                    if ((curlen + i->length()) > attr.maxtotal)
                    {
//                        debug_msg("pushing back '%s'", cur.c_str());
                        buffer.push_back(cur);
                        cur.clear();

                        /* indent */
                        while (cur.length() < attr.maxlabel)
                            cur.append(" ");
                    }
            
                    if (highlight_found)
                    {
                        cur += attr.highlight_color + (*i) + attr.no_color + " ";

                        /* restore saved maxtotal */
                        attr.maxtotal = oldlen;
                    }
                    else if (marked_away)
                    {
                        cur += (*i) + attr.devaway_color + "*" + attr.no_color + " ";
                        attr.maxtotal = oldlen;
                    }
                    else
                        cur += *i + " ";
                }
            }
        }
    }

    if (cur.length() > 0)
    {
//        debug_msg("pushing back '%s'", cur.c_str());
        buffer.push_back(cur);
    }
}
/****************************************************************************
 * Flush our buffer's contents to the specified stream,
 * removing each element in the buffer as we do so.
 ***************************************************************************/
void
formatter_T::flush(std::ostream &stream)
{
//    std::remove_copy(buffer.begin(), buffer.end(),
//        std::ostream_iterator<string_type>(stream, "\n"),
//        "supercalifragilisticexpialidocious");

    buffer_type::iterator i;
    for (i = this->buffer.begin() ; i != this->buffer.end() ; ++i)
        stream << *i << std::endl;

    this->buffer.clear();
}
/****************************************************************************/

/* vim: set tw=80 sw=4 et : */
