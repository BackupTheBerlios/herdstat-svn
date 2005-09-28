/*
 * herdstat -- src/options.hh
 * $Id$
 * Copyright (c) 2005 Aaron Walker <ka0ttic at gentoo.org>
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

#ifndef HAVE_OPTIONS_HH
#define HAVE_OPTIONS_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string>
#include <vector>
#include <herdstat/portage/config.hh>

enum options_action_T
{
    action_unspecified,
    action_herd,
    action_dev,
    action_pkg,
    action_meta,
    action_stats,
    action_which,
    action_versions,
    action_find,
    action_away,
    action_fetch,
    action_bday
};

class options
{
    public:
        options();

        static bool verbose() { return _verbose; }
        static void set_verbose(bool v) { _verbose = v; }
        static bool quiet() { return _quiet; }
        static void set_quiet(bool v) { _quiet = v; }
        static bool debug() { return _debug; }
        static void set_debug(bool v) { _debug = v; }
        static bool timer() { return _timer; }
        static void set_timer(bool v) { _timer = v; }
        static bool all() { return _all; }
        static void set_all(bool v) { _all = v; }
        static bool dev() { return _dev; }
        static void set_dev(bool v) { _dev = v; }
        static bool count() { return _count; }
        static void set_count(bool v) { _count = v; }
        static bool color() { return _color; }
        static void set_color(bool v) { _color = v; }
        static bool overlay() { return _overlay; }
        static void set_overlay(bool v) { _overlay = v; }
        static bool eregex() { return _eregex; }
        static void set_eregex(bool v) { _eregex = v; }
        static bool regex() { return _regex; }
        static void set_regex(bool v) { _regex = v; }
        static bool qa() { return _qa; }
        static void set_qa(bool v) { _qa = v; }
        static bool meta() { return _meta; }
        static void set_meta(bool v) { _meta = v; }
        static bool metacache() { return _metacache; }
        static void set_metacache(bool v) { _metacache = v; }
        static bool querycache() { return _querycache; }
        static void set_querycache(bool v) { _querycache = v; }
        static bool devaway() { return _devaway; }
        static void set_devaway(bool v)  { _devaway = v; }
        
        static int& querycache_max() { return _querycache_max; }
        static void set_querycache_max(int v) { _querycache_max = v; }
        static long& querycache_expire() { return _querycache_expire; }
        static void set_querycache_expire(long v) { _querycache_expire = v; }
        static long& devaway_expire() { return _devaway_expire; }
        static void set_devaway_expire(long v) { _devaway_expire = v; }
        static size_t& maxcol() { return _maxcol; }
        static void set_maxcol(size_t v) { _maxcol = v; }

        static std::ostream * const outstream() { return _outstream; }
        static void set_outstream(std::ostream *s) { _outstream = s; }
        static const std::string& outfile() { return _outfile; }
        static void set_outfile(const std::string& v) { _outfile.assign(v); }
        static const std::string& cvsdir() { return _cvsdir; }
        static void set_cvsdir(const std::string& v) { _cvsdir.assign(v); }
        static const std::string& herdsxml() { return _herdsxml; }
        static void set_herdsxml(const std::string& v) { _herdsxml.assign(v); }
        static const std::string& devawayxml() { return _devawayxml; }
        static void set_devawayxml(const std::string& v) { _devawayxml.assign(v); }
        static const std::string& userinfoxml() { return _userinfoxml; }
        static void set_userinfoxml(const std::string& v) { _userinfoxml.assign(v); }
        static const std::string& with_herd() { return _with_herd; }
        static void set_with_herd(const std::string& v) { _with_herd.assign(v); }
        static const std::string& with_dev() { return _with_dev; }
        static void set_with_dev(const std::string& v) { _with_dev.assign(v); }
        static const std::string& localstatedir() { return _localstatedir; }
        static void set_localstatedir(const std::string& v) { _localstatedir.assign(v); }
        static const std::string& wget_options() { return _wgetopts; }
        static void set_wget_options(const std::string& v) { _wgetopts.assign(v); }
        static const std::string& labelcolor() { return _labelcolor; }
        static void set_labelcolor(const std::string& v) { _labelcolor.assign(v); }
        static const std::string& hlcolor() { return _hlcolor; }
        static void set_hlcolor(const std::string& v) { _hlcolor.assign(v); }
        static const std::string& metacache_expire() { return _metacache_expire; }
        static void set_metacache_expire(const std::string& v) { _metacache_expire.assign(v); }
        static const std::string& highlights() { return _highlights; }
        static void set_highlights(const std::string& v) { _highlights.assign(v); }
        static const std::string& locale() { return _locale; }
        static void set_locale(const std::string& v) { _locale.assign(v); }
        static const std::string& field() { return _field; }
        static void set_field(const std::string& v) { _field.assign(v); }

        static options_action_T action() { return _action; }
        static void set_action(options_action_T v) { _action = v; }

        /* read-only */
        static const std::string& portdir() { return _portdir; }
        static const std::vector<std::string>& overlays() { return _overlays; }

    private:
        static bool _init;
        static bool _verbose;
        static bool _quiet;
        static bool _debug;
        static bool _timer;
        static bool _all;
        static bool _dev;
        static bool _count;
        static bool _color;
        static bool _overlay;
        static bool _eregex;
        static bool _regex;
        static bool _qa;
        static bool _meta;
        static bool _metacache;
        static bool _querycache;
        static bool _devaway;

        static int _querycache_max;
        static long _querycache_expire;
        static long _devaway_expire;
        static size_t _maxcol;

        static std::ostream *_outstream;
        static std::string _outfile;
        static std::string _cvsdir;
        static std::string _herdsxml;
        static std::string _devawayxml;
        static std::string _userinfoxml;
        static std::string _with_herd;
        static std::string _with_dev;
        static std::string _localstatedir;
        static std::string _wgetopts;
        static std::string _labelcolor;
        static std::string _hlcolor;
        static std::string _metacache_expire;
        static std::string _highlights;
        static std::string _locale;
        static std::string _field;

        static options_action_T _action;

        static portage::config _config;
        static const std::string& _portdir;
        static const std::vector<std::string>& _overlays;
};

#endif

/* vim: set tw=80 sw=4 et : */
