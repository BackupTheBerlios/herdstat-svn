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

std::string
Formatter::highlight(const std::vector<std::string>& data)
{
    std::string result;
    std::vector<std::string>::const_iterator i, end;
    for (i = data.begin(), end = data.end() ; i != end ; ++i)
    {
        bool highlight = false;

        /* perform any highlight substitutions */
        std::map<std::string, std::string>::const_iterator h, hend;
        for (h = _attrs.highlights().begin(), hend = _attrs.highlights().end();
                h != hend ; ++h)
        {

#define MARK_IF_AWAY(x) \
            if (not _attrs.quiet() and\
                std::find(_attrs.devaway().begin(), _attrs.devaway().end(), \
                    util::strip_colors(x)) != _attrs.devaway().end()) { \
                result += _attrs.devaway_color() + "*"; \
                _attrs.set_marked_away(true); \
            }

            std::string::size_type pos = h->first.find("re:");
            if (pos == std::string::npos and h->first == *i)
            {
                highlight = true;
                if (result.find((*i) + _attrs.no_color()) == std::string::npos)
                {
                    result += h->second + (*i);
                    MARK_IF_AWAY(*i);
                    result += _attrs.no_color() + " ";
                }
                else
                {
                    result += (*i);
                    MARK_IF_AWAY(*i);
                    result += _attrs.no_color() + " ";
                }
            }
            else if (pos != std::string::npos)
            {
                util::Regex regex(h->first.substr(pos+3));
                if (regex == *i)
                {
                    highlight = true;
                    if (result.find((*i) + _attrs.no_color()) == std::string::npos)
                    {
                        result += h->second + (*i);
                        MARK_IF_AWAY(*i);
                        result += _attrs.no_color() + " ";
                    }
                    else
                    {
                        result += (*i);
                        MARK_IF_AWAY(*i);
                        result += _attrs.no_color() + " ";
                    }
                }
            }
        }

        if (not highlight)
        {
            result += (*i);
            MARK_IF_AWAY(*i);
            result += _attrs.no_color() + " ";
        }

#undef MARK_IF_AWAY

    }

    if (result[result.length() - 1] == ' ')
        result.erase(result.length() - 1);

    return result;
}

void
Formatter::operator()(const std::string& label,
                      const std::vector<std::string>& data)
{
    if (_attrs.quiet() and _attrs.quiet_delim() == " ")
        this->append("", util::join(data));
    else if (_attrs.quiet())
    {
//        std::transform(data.begin(), data.end(),
//                std::back_inserter(_buffer), EmptyFirst());
    
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
    std::size_t maxdata  = _attrs.maxlen() - maxlabel;
    std::size_t maxlen = maxlabel + maxdata;

    for (i = _buffer.begin() ; i != _buffer.end() ; ++i)
    {
        std::string out, label, data;
        std::size_t clen = 0; /* color length */

        /* label */
        if (i->first.empty() and not i->second.empty() and not _attrs.quiet())
        {
            label.assign(maxlabel, ' ');
        }
        else if (not i->first.empty() and not _attrs.quiet())
        {
            if (_attrs.colors())
            {
                label.assign(_attrs.label_color() + i->first + _attrs.no_color() + ":");
                clen += _attrs.label_color().length() + _attrs.no_color().length();
                label.append(maxlabel - i->first.length(), ' ');
            }
            else
            {
                label.assign(i->first + ":");
                label.append(maxlabel - i->first.length(), ' ');
            }
        }

        out += label;

        /* data */
        if (not i->second.empty())
        {
            std::vector<std::string> parts(util::split(i->second));
            data.assign(highlight(parts));

            std::string colorfree(util::strip_colors(data));
            if (_attrs.quiet() or not _attrs.colors())
                data.assign(colorfree);
            clen += (data.length() - colorfree.length());

            /* if it'll all fit on one line or we cant break it up, do it */
            std::string::size_type breakpos;
            if (((label.length() + data.length() - clen) < maxlen) or
                ((breakpos = data.rfind(" ", maxdata-2)) == std::string::npos))
                out += data;
            /* otherwise, truncate it and loop through the rest
             * of the words, wrapping when necessary */
            else if (breakpos != std::string::npos)
            {
                out += data.substr(0, breakpos) + "\n";
                data.erase(0, breakpos+1);
                parts = util::split(data);

                label.assign(maxlabel+1, ' '); /* indent */
                out += label;

                std::size_t outlen = label.length(); /* current output length */
                while (not parts.empty())
                {
                    colorfree.assign(util::strip_colors(parts.front()));
                    clen = (parts.front().length() - colorfree.length());

                    if (((outlen + parts.front().length() + 1) - clen) < maxlen)
                    {
                        out += parts.front() + " ";
                        outlen += colorfree.length() + 1;
                    }
                    else
                    {
                        out += "\n";
                        label.assign(maxlabel+1, ' ');
                        out += label + parts.front() + " ";
                        outlen = label.length() + colorfree.length() + 1;
                    }

                    parts.erase(parts.begin());
                }
            }
        }

        outbuf.push_back(out);
    }

    _buffer.clear();

    /* display it finally */
    std::copy(outbuf.begin(), outbuf.end(),
        std::ostream_iterator<std::string>(stream, "\n"));
}

/* vim: set tw=80 sw=4 et : */
