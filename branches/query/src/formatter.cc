/*
 * herdstat -- src/format.cc
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

#include <iostream>
#include <algorithm>
#include <functional>
#include <iterator>

#include <herdstat/exceptions.hh>
#include <herdstat/util/string.hh>
#include <herdstat/util/regex.hh>

#include "common.hh"
#include "formatter.hh"

using namespace herdstat;

struct EmptyFirst
{
    std::pair<std::string, std::string>
    operator()(const std::string& str) const
    { return std::pair<std::string, std::string>("", str); }
};

FormatAttrs::FormatAttrs()
    : _cmap(), _quiet(false), _colors(true), _away(false), _quiet_delim("\n"),
      _maxlen(78),
      _lcolor(_cmap[green]), _hcolor(_cmap[yellow]), _dcolor(_cmap[red]),
      _no_color(_cmap[none]), _devaway(), _highlights()
{
}

void
FormatAttrs::add_highlights(const std::vector<std::string>& pairs)
{
    std::vector<std::string>::const_iterator i;
    for (i = pairs.begin() ; i != pairs.end() ; ++i)
    {
        std::vector<std::string> parts(util::split(*i, ','));
        if (parts.size() == 1)
            _highlights.insert(std::make_pair(parts.front(), _hcolor));
        else if (parts.size() == 2)
            _highlights.insert(std::make_pair(parts.front(),
                    _colors ? _cmap[parts.back()] : ""));
        else
            throw Exception("Invalid highlight specification '%s'", i->c_str());
    }
}

Formatter::Formatter()
{
}

Formatter::Formatter(const FormatAttrs& attrs)
{
    set_attrs(attrs);
}

void
Formatter::highlight(std::vector<std::string>& data)
{
    std::vector<std::string>::iterator i;
    for (i = data.begin() ; i != data.end() ; ++i)
    {
        const std::string colorfree(util::strip_colors(*i));
        bool is_away = (not _attrs.quiet() and
                std::find(_attrs.devaway().begin(), _attrs.devaway().end(),
                colorfree) != _attrs.devaway().end());

        if (is_away)
            _attrs.set_marked_away(true);

        std::string result;

        /* search highlights */
        std::map<std::string, std::string>::const_iterator h;
        for (h = _attrs.highlights().begin() ;
             h != _attrs.highlights().end() ; ++h)
        {
            /* if it's a regex and the regex matches, or
             * if its not a regex and the literal matches, highlight it */
            std::string::size_type pos = h->first.find("re:");
            if ((pos != std::string::npos and
                 util::Regex(h->first.substr(pos+3)) == colorfree) or
                (pos == std::string::npos and h->first == colorfree))
            {
                if (result.find((*i)+_attrs.no_color()) == std::string::npos)
                    result += h->second + (*i);
                else
                    result += (*i);

                if (is_away)
                    result += _attrs.devaway_color() + "*";

                if (_attrs.colors())
                    result += _attrs.no_color();
            }
        }

        /* no highlights were found, so fill result with *i and mark
         * it if the word matches one in devaway */
        if (result.empty())
        {
            result += (*i);
            if (is_away)
            {
                result += _attrs.devaway_color() + "*";
                if (_attrs.colors())
                    result += _attrs.no_color();
            }
        }

        i->assign(result);
    }
}

void
Formatter::operator()(const std::string& label,
                      const std::vector<std::string>& data)
{
    if (_attrs.quiet() and _attrs.quiet_delim() == " ")
        this->append("", util::join(data));
    else if (_attrs.quiet())
    {
        std::vector<std::string>::const_iterator i;
        for (i = data.begin() ; i != data.end() ; ++i)
            this->append("", *i);
    }
    else
        this->operator()(label, util::join(data));
}

void
Formatter::flush(std::ostream& stream)
{
    if (_buffer.empty())
        return;

    std::vector<std::string> outbuf;

    /* get max label length */
    buffer_type::iterator i =
        std::max_element(_buffer.begin(), _buffer.end(), FirstLengthLess());

    std::size_t maxlabel = i->first.length();
    if (not _attrs.quiet()) maxlabel += 3; /* indent */
    std::size_t maxlen = _attrs.maxlen();

    for (i = _buffer.begin() ; i != _buffer.end() ; ++i)
    {
        std::string out, label, data;

        /* label */
        if (i->first.empty() and not i->second.empty() and not _attrs.quiet())
            label.assign(maxlabel+1, ' ');
        else if (not i->first.empty() and not _attrs.quiet())
        {
            if (_attrs.colors())
                label.assign(_attrs.label_color() + i->first + _attrs.no_color() + ":");
            else
                label.assign(i->first + ":");

            label.append(maxlabel - i->first.length(), ' ');
        }

        out += label;

        /* data */
        if (not i->second.empty())
        {
            /* perform highlights and get back a vector of the data */
            std::vector<std::string> parts(util::split(i->second));
            highlight(parts);

            std::string::size_type outlen = util::strip_colors(label).length();
            while (not parts.empty())
            {
                std::string& word(parts.front());
                std::string wcfree(util::strip_colors(word));

                if ((outlen + wcfree.length()) < maxlen)
                {
                    out += word + " ";
                    outlen += wcfree.length() + 1;
                }
                else
                {
                    if (out[out.length() - 1] == ' ')
                        out.erase(out.length() - 1);

                    out += "\n";
                    label.assign(maxlabel+1, ' ');
                    out += label + word + " ";
                    outlen = label.length() + wcfree.length() + 1;
                }

                parts.erase(parts.begin());
            }
        }

        if (out[out.length() - 1] == ' ')
            out.erase(out.length() - 1);

        outbuf.push_back(out);
    }

    _buffer.clear();

    /* display it finally */
    std::copy(outbuf.begin(), outbuf.end(),
        std::ostream_iterator<std::string>(stream, "\n"));
}

/* vim: set tw=80 sw=4 et : */
