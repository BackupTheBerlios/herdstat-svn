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
#include <iterator>

#include "formatter.hh"

/** static members **********************************************************/
formatter_T::attrs_type  formatter_T::attr;
formatter_T::buffer_type formatter_T::buffer;
/****************************************************************************/
formatter_T::attrs_type::attrs_type()
{
    colors = quiet = false;
    maxtotal = 78;
    maxlabel = 20;
    maxdata  = 58;
}
/****************************************************************************
 * Finalize the attributes ; calculate increase in string length
 * because of the colors, etc. This is called once by the caller
 * after the attrs have been set.
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
        color_type color;
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
        attr.maxclabel = attr.maxlabel;
        attr.maxcdata  = attr.maxdata;
        attr.maxctotal = attr.maxtotal;
    }
}
/****************************************************************************
 * Given a vector of data, see if any of the elements
 * match words that should be highlighted.  Return the
 * string, highlighted or not.
 ****************************************************************************/
util::string
formatter_T::highlight(const std::vector<util::string> &data)
{
    util::string s;
    std::vector<util::string>::const_iterator i;

    if (not colors())
        attr.highlight_color.clear();

    for (i = data.begin() ; i != data.end() ; ++i)
    {
        if (std::find(attr.highlights.begin(),
            attr.highlights.end(), *i) != attr.highlights.end())
            s += attr.highlight_color + (*i) + attr.no_color + " ";
        else
            s += *i + " ";
    }

    return s;
}
/****************************************************************************/
void
formatter_T::append(const util::string &label,
                    const std::vector<util::string> &data)
{
    /* if quiet, handle it here, as we're going to end up splitting
     * the data string into a vector anyways */
    if (attr.quiet and attr.quiet_delim == " ")
        buffer.push_back(util::stringify(data));

    else if (attr.quiet)
        std::copy(data.begin(), data.end(), std::back_inserter(buffer));

    /* otherwise, produce a data string and call the real append() */
    else
        append(label, util::stringify(data));
}
/****************************************************************************
 * Append text to the output buffer.  Smartly handle lines that are bigger
 * than maxcol (new line and indent), and make sure it all aligns nicely.
 ****************************************************************************/
void
formatter_T::append(const util::string &label, const util::string &data)
{
    util::string cur;

    /* don't display the label (or indent) if the quiet attr is set */
    if (not quiet())
    {
        if (label.length() > attr.maxlabel)
            throw format_E("Label '%s' is greater than maxlabel attribute (%d)",
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
        debug_msg("data = '%s'", data.c_str());

        if (quiet())
            append(label, data.split());
        else
        {
            size_type maxlen =
                (cur.find("\033") == util::string::npos ?
                attr.maxtotal : attr.maxctotal);

            if ((cur.length() + data.length()) < maxlen)
            {
                cur += highlight(data.split());
                debug_msg("it all fits on one line");
            }
            else
            {
                /* line's full, so find a location where we can truncate */
                util::string::size_type pos = data.rfind(" ", attr.maxdata);
                if (pos == util::string::npos)
                    cur += highlight(data.split());
                else
                    cur += highlight(util::string(data.substr(0, pos)).split());

                debug_msg("pushing back '%s'", cur.c_str());
                buffer.push_back(cur);
                cur.clear();

                if (pos == util::string::npos)
                    return;

                /* indent */
                while (cur.length() < attr.maxlabel)
                    cur.append(" ");

                debug_msg("handling leftovers '%s'", data.substr(pos).c_str());

                /* handle leftovers */
                std::vector<util::string> leftovers = util::string(data.substr(pos)).split();

                std::vector<util::string>::iterator i;
                for (i = leftovers.begin() ; i != leftovers.end() ; ++i)
                {
                    util::string::size_type oldlen = 0;
                    bool highlight_found = false;

                    /* should the current word be highlighted? */
                    if (std::find(attr.highlights.begin(),
                        attr.highlights.end(), *i) != attr.highlights.end())
                    {
                        highlight_found = true;

                        /* adjust maxtotal appropriately */
                        oldlen = attr.maxtotal;
                        attr.maxtotal = oldlen + attr.highlight_color.length() +
                            attr.no_color.length();
                    }

                    util::string::size_type curlen;

                    /*
                     * Does cur contain a previously highlighted word? If so,
                     * we need to compensate for the color lengths.
                     */
                    if ((cur.find("\033") != util::string::npos))
                    {
                        curlen = cur.length() - attr.highlight_color.length() -
                            attr.no_color.length();

                        debug_msg("found color in cur; setting curlen to %d",
                            curlen);
                    }

                    /* compensate for current highlight? */
                    else if (highlight_found)
                    {
                        curlen = cur.length() + attr.highlight_color.length() +
                            attr.no_color.length();

                        debug_msg("highlighted '%s'; setting curlen to %d",
                            i->c_str(), curlen);
                    }

                    /* don't compensate */
                    else
                    {
                        curlen = cur.length();
                        debug_msg("no highlight; setting curlen to %d", curlen);
                    }

                    /* does it fit on the current line? */
                    if ((curlen + i->length()) > attr.maxtotal)
                    {
                        debug_msg("pushing back '%s'", cur.c_str());
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
                    else
                        cur += *i + " ";
                }
            }
        }
    }

    if (cur.length() > 0)
    {
        debug_msg("pushing back '%s'", cur.c_str());
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
//        std::ostream_iterator<util::string>(stream, "\n"),
//        "supercalifragilisticexpialidocious");

    buffer_type::iterator i;
    for (i = this->buffer.begin() ; i != this->buffer.end() ; ++i)
    {
        try
        {
            stream << *i << std::endl;
        }
        catch (const Glib::ConvertError &e)
        {
            stream
                << "pfft. glib found a utf8 char and puked (gnome bug 301935)."
                << std::endl;
            continue;
        }
    }
}
/****************************************************************************/

/* vim: set tw=80 sw=4 et : */
