/*
 * herdstat -- src/io/action/set.cc
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

#include <herdstat/util/string.hh>
#include "handler_map.hh"
#include "formatter.hh"
#include "io/action/set.hh"

using namespace herdstat;

const char * const
SetIOActionHandler::id() const
{
    return "set";
}

const char * const
SetIOActionHandler::desc() const
{
    return "Set an option.";
}

const char * const
SetIOActionHandler::usage() const
{
    return "set <option=value>";
}

void
SetIOActionHandler::generate_completions(std::vector<std::string> *v) const
{
    static const char *comps[] =
    {
        "action",
        "quiet",
        "color",
        "verbose",
        "overlay",
        "regex",
        "eregex",
        "qa",
        "metacache",
        "devaway",
        "meta",
        "dev",
        "maxcol",
        "cvsdir",
        "herdsxml",
        "devawayxml",
        "userinfoxml",
        "localstatedir",
        "locale",
        "labelcolor",
        "hlcolor",
        "with_dev",
        "with_herd",
        "metacache_expire"
    };

    v->assign(comps, comps+NELEMS(comps));
}

void
SetIOActionHandler::do_results(Query& query,
                               QueryResults * const results)
{
    for (Query::iterator q = query.begin() ; q != query.end() ; ++q)
    {
        try
        {
            std::vector<std::string> parts;
            util::split(q->second, std::back_inserter(parts), "=");
            if (parts.size() != 2)
                throw Exception("Failed to parse '%s'. Use option=value.",
                                q->second.c_str());
        
            const std::string& key(parts.front());
            std::string& val(parts.back());

            /* remove any begin/end quotes */
            const char begin(val[0]), end(val[val.length() - 1]);
            if (begin == '\'' or begin == '"')
                val.erase(0, 1);
            if (end == '\'' or end == '"')
                val.erase(val.length() - 1);

            FormatAttrs& attrs(GlobalFormatter().attrs());

#define SET_INT_IF_EQUAL(type, x) \
            if (key == #x) options.set_##x(util::destringify<type>(val));
#define SET_STR_IF_EQUAL(x) \
            if (key == #x) options.set_##x(val);

            if (key == "action")
            {
                if (GlobalHandlerMap<ActionHandler>().find(val) !=
                    GlobalHandlerMap<ActionHandler>().end() or
                    val == "unspecified")
                    options.set_action(val);
                else
                    throw Exception("Unknown action '"+val+"'");
            }
            else if (key == "quiet")
            {
                const bool v(util::destringify<bool>(val));
                options.set_quiet(v);
                options.set_color(not v);
                attrs.set_quiet(v);
            }
            else if (key == "color" or key == "colors")
            {
                const bool v(util::destringify<bool>(val));
                options.set_color(v);
                attrs.set_colors(v);
                util::ColorMap& color(GlobalColorMap());
                attrs.set_label_color(color[options.labelcolor()]);
                attrs.set_highlight_color(color[options.hlcolor()]);
            }
            else SET_INT_IF_EQUAL(bool, verbose)
            else SET_INT_IF_EQUAL(bool, overlay)
            else SET_INT_IF_EQUAL(bool, regex)
            else if (key == "eregex")
            {
                const bool v(util::destringify<bool>(val));
                options.set_regex(v);
                options.set_eregex(v);
            }
            else SET_INT_IF_EQUAL(bool, qa)
            else SET_INT_IF_EQUAL(bool, metacache)
            else SET_INT_IF_EQUAL(bool, querycache)
            else SET_INT_IF_EQUAL(bool, devaway)
            else SET_INT_IF_EQUAL(bool, meta)
            else SET_INT_IF_EQUAL(bool, dev)
            else SET_INT_IF_EQUAL(int, querycache_max)
            else SET_INT_IF_EQUAL(long, querycache_expire)
            else SET_INT_IF_EQUAL(size_t, maxcol)
            else SET_STR_IF_EQUAL(cvsdir)
            else SET_STR_IF_EQUAL(herdsxml)
            else SET_STR_IF_EQUAL(devawayxml)
            else SET_STR_IF_EQUAL(userinfoxml)
            else SET_STR_IF_EQUAL(localstatedir)
            else SET_STR_IF_EQUAL(locale)
            else SET_STR_IF_EQUAL(labelcolor)
            else SET_STR_IF_EQUAL(hlcolor)
            else SET_STR_IF_EQUAL(with_dev)
            else SET_STR_IF_EQUAL(with_herd)
            else SET_STR_IF_EQUAL(metacache_expire)
            else results->add("Unknown option '" + key + "'.");

#undef SET_INT_IF_EQUAL
#undef SET_STR_IF_EQUAL
        }
        catch (const Exception& e)
        {
            results->add(e.what());
        }
    }
}

/* vim: set tw=80 sw=4 fdm=marker et : */
