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
#include <herdstat/util/functional.hh>

#include "common.hh"
#include "formatter.hh"

using namespace herdstat;
/****************************************************************************
 * FormatAttrs - Format attributes
 ****************************************************************************/
FormatAttrs::FormatAttrs()
    : _cmap(), _quiet(false), _colors(true), _away(false), _quiet_delim("\n"),
      _maxlen(78),
      _lcolor(_cmap[green]), _hcolor(_cmap[yellow]), _dcolor(_cmap[red]),
      _no_color(_cmap[none]), _devaway(), _highlights()
{
}
/****************************************************************************/
void
FormatAttrs::add_highlights(const std::vector<std::string>& pairs)
{
    std::vector<std::string>::const_iterator i;
    for (i = pairs.begin() ; i != pairs.end() ; ++i)
    {
        std::vector<std::string> parts(util::split(*i, ','));
        if (parts.size() == 1)
            _highlights.insert(std::make_pair(
                    util::Regex(parts.front()), _hcolor));
        else if (parts.size() == 2)
            _highlights.insert(std::make_pair(
                    util::Regex(parts.front()),
                    _colors ? _cmap[parts.back()] : ""));
        else
            throw Exception("Invalid highlight specification '%s'", i->c_str());
    }
}
/****************************************************************************
 * Small struct for encapsulating some data to pass to Wrap().
 ****************************************************************************/
struct OutData
{
    OutData(std::string::size_type maxln,
            std::string::size_type maxlb)
        : str(), len(0), maxlen(maxln), maxlabel(maxlb) { }

    std::string str;
    std::string::size_type len;
    const std::string::size_type maxlen;
    const std::string::size_type maxlabel;
};
/****************************************************************************
 * Function object for highlighting words based on user-defined regular
 * expressions.
 ****************************************************************************/
struct Highlight
    : std::binary_function<std::string, FormatAttrs * const, std::string>
{
    std::string
    operator()(const std::string& str, FormatAttrs * const attrs) const;

    /* handle special cases where we don't
     * want to highlight certain characters in a word */
    std::string handle_special_cases(const std::string& str,
                                     const std::string& color,
                                     const std::string& nocolor) const;
};
/****************************************************************************
 * Function object for performing line wrapping.
 ****************************************************************************/
struct Wrap
    : std::binary_function<std::string, OutData * const, void>
{
    void operator()(const std::string& str, OutData * const out) const;
};
/****************************************************************************
 * Function object for formatting pairs of strings (label/data).
 ****************************************************************************/
struct Format : std::binary_function<std::pair<std::string, std::string>,
                                     FormatAttrs * const, std::string>
{
    std::string
    operator()(const std::pair<std::string, std::string>& data,
               FormatAttrs * const attrs) const;
};
/****************************************************************************/
std::string
Highlight::handle_special_cases(const std::string& str,
                                const std::string& color,
                                const std::string& nocolor) const
{
    if (str.empty())
        return str;

    /* wrapped in parenthesis */
    if (str[0] == '(' and str[str.length() - 1] == ')')
        return std::string("(" + color + str.substr(1, str.length() - 2) +
                nocolor + ")");
    /* ends in a comma */
    else if (str[str.length() - 1] == ',')
        return std::string(color + str.substr(0, str.length() - 1) +
                nocolor + ",");

    /* non-special case - highlight whole word */
    return (color+str);
}
/****************************************************************************/
std::string
Highlight::operator()(const std::string& str, FormatAttrs * const attrs) const
{
    const std::string colorfree(util::strip_colors(str));
    const bool is_away(not attrs->quiet() and
        std::binary_search(attrs->devaway().begin(),
            attrs->devaway().end(), colorfree));

    if (is_away)
        attrs->set_marked_away(true);

    /* Does str (stripped of any colors) match any regular expressions in
     * the highlights map? */
    util::RegexMap<std::string>::const_iterator h =
        attrs->highlights().find(colorfree);
    if (h != attrs->highlights().end())
    {
        std::string result(
                handle_special_cases(str, h->second, attrs->no_color()));
        if (is_away) result.append(attrs->devaway_color()+"*");
        return (result+attrs->no_color());
    }

    /* wasnt found in highlights, so mark it if away, or return
     * the original string. */
    std::string result(str);
    if (is_away)
        result.append(attrs->devaway_color()+"*"+attrs->no_color());

    return result;
}

void
Wrap::operator()(const std::string& str, OutData * const out) const
{
    /* length of str after colors are stripped */
    const std::string::size_type wclen =
        util::strip_colors(str).length();

    /* if it fits or it doesnt fit but it's the only word (a long
     * URL for example), put it on the current line */
    if (((out->len + wclen) < out->maxlen) or
        (out->len == (out->maxlabel+1)))
    {
        out->str.append(str + " ");
        out->len += wclen + 1;
    }
    /* otherwise, end that line and start a new one */
    else
    {
        if (out->str[out->str.length() - 1] == ' ')
            out->str.erase(out->str.length() - 1);

        out->str.append("\n");
        out->str.append(out->maxlabel+1, ' ');
        out->str.append(str + " ");
        out->len = out->maxlabel+1 + wclen+1;
    }
}

std::string
Format::operator()(const std::pair<std::string, std::string>& pair,
                   FormatAttrs * const attrs) const
{
    OutData out(attrs->maxlen(), attrs->maxlabel());
    const std::string& label(pair.first);
    const std::string& data(pair.second);

    /* handle label */
    if (label.empty() and not data.empty() and not attrs->quiet())
        out.str.assign(attrs->maxlabel()+1, ' ');
    else if (not label.empty() and not attrs->quiet())
    {
        out.str.assign(attrs->label_color() + label + attrs->no_color() + ":");
        out.str.append(attrs->maxlabel() - label.length(), ' ');
    }

    /* handle data */
    if (not data.empty())
    {
        /* split into a vector of words */
        std::vector<std::string> parts(util::split(data));
        /* perform any highlights */
        std::transform(parts.begin(), parts.end(),
            parts.begin(), std::bind2nd(Highlight(), attrs));
        /* perform any line wrapping */
        out.len = util::strip_colors(out.str).length();
        std::for_each(parts.begin(), parts.end(),
            std::bind2nd(Wrap(), &out));
    }

    if (out.str[out.str.length() - 1] == ' ')
        out.str.erase(out.str.length() - 1);

    return out.str;
}

/*
 * Append a label/vector of data.
 */

void
Formatter::operator()(const std::string& label,
                      const std::vector<std::string>& data)
{
    if (_attrs.quiet() and _attrs.quiet_delim() != " ")
        /* append a new element (with an empty label) for
         * each element in data to our buffer */
        std::transform(data.begin(), data.end(),
                std::back_inserter(_buffer), util::EmptyFirst());
    else
        this->operator()(label, util::join(data));
}

/*
 * Flush our buffer.
 * This is the user interface to what does the actual formatting.
 */

void
Formatter::flush(std::ostream& stream)
{
    if (_buffer.empty())
        return;

    /* dynamically set maximum label length
     * to at least the size of the longest label */
    buffer_type::iterator i =
        std::max_element(_buffer.begin(), _buffer.end(), FirstLengthLess());

    _attrs.set_maxlabel(_attrs.quiet() ?
                            i->first.length() :
                            i->first.length() + 3 /* padding */);

    /* for each element in our buffer, format it and insert
     * the result into the real output buffer. */
    std::vector<std::string> outbuf;
    std::transform(_buffer.begin(), _buffer.end(),
        std::back_inserter(outbuf),
        std::bind2nd(Format(), &_attrs));

    _buffer.clear();

    /* display it finally */
    std::copy(outbuf.begin(), outbuf.end(),
        std::ostream_iterator<std::string>(stream, "\n"));
}

/* vim: set tw=80 sw=4 fdm=marker et : */
