/*
 * herdstat -- src/overlaydisplay.cc
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

#include <herdstat/util/misc.hh>
#include <herdstat/util/string.hh>

#include "common.hh"
#include "query_results.hh"
#include "overlaydisplay.hh"

using namespace herdstat;

OverlayDisplay::OverlayDisplay(QueryResults * const results)
    : _options(GlobalOptions()), _results(results)
{
}

OverlayDisplay::~OverlayDisplay()
{
    if (_options.quiet() or _oset.empty())
        return;

    _results->add_linebreak();
    _results->add("Portage overlays:");

    for (iterator i = _oset.begin() ; i != _oset.end() ; ++i)
        _results->add(" " + this->operator[](i->first) + " " + i->first);
}

std::string
OverlayDisplay::operator[] (const std::string& overlay) const
{
    if (_options.quiet())
        return std::string();

    /* cant use find() as we dont now the second value of the pair */
    iterator i;
    for (i = _oset.begin() ; i != _oset.end() ; ++i)
        if (i->first == overlay) break;
    assert(i != _oset.end());

    if (_options.color())
    {
        util::ColorMap& color(GlobalColorMap());
        return util::sprintf("%s[%d]%s", color[cyan].c_str(),
                i->second, color[none].c_str());
    }

    return util::sprintf("[%d]", i->second);
}

void
OverlayDisplay::insert(const std::string& overlay)
{
    /* cant use find() as we dont now the second value of the pair */
    for (iterator i = _oset.begin() ; i != _oset.end() ; ++i)
        if (i->first == overlay) return;

    _oset.insert(value_type(overlay, _oset.size() + 1));
}

/* vim: set tw=80 sw=4 fdm=marker et : */
