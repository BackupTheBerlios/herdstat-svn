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
#include <utility>
#include <herdstat/noncopyable.hh>

typedef std::vector<std::pair<std::string, std::string> > fields_type;

class Options : private herdstat::noncopyable
{
    public:
        bool verbose() const { return _verbose; }
        void set_verbose(bool v) { _verbose = v; }
        bool quiet() const { return _quiet; }
        void set_quiet(bool v) { _quiet = v; }
        bool debug() const { return _debug; }
        void set_debug(bool v) { _debug = v; }
        bool timer() const { return _timer; }
        void set_timer(bool v) { _timer = v; }
        bool all() const { return _all; }
        void set_all(bool v) { _all = v; }
        bool dev() const { return _dev; }
        void set_dev(bool v) { _dev = v; }
        bool count() const { return _count; }
        void set_count(bool v) { _count = v; }
        bool color() const { return _color; }
        void set_color(bool v) { _color = v; }
        bool overlay() const { return _overlay; }
        void set_overlay(bool v) { _overlay = v; }
        bool eregex() const { return _eregex; }
        void set_eregex(bool v) { _eregex = v; }
        bool regex() const { return _regex; }
        void set_regex(bool v) { _regex = v; }
        bool qa() const { return _qa; }
        void set_qa(bool v) { _qa = v; }
        bool meta() const { return _meta; }
        void set_meta(bool v) { _meta = v; }
        bool metacache() const { return _metacache; }
        void set_metacache(bool v) { _metacache = v; }
        bool querycache() const { return _querycache; }
        void set_querycache(bool v) { _querycache = v; }
        bool devaway() const { return _devaway; }
        void set_devaway(bool v)  { _devaway = v; }
        bool fetch() const { return _fetch; }
        void set_fetch(bool v) { _fetch = v; }
        
        const int& querycache_max() const { return _querycache_max; }
        void set_querycache_max(int v) { _querycache_max = v; }
        const long& querycache_expire() const { return _querycache_expire; }
        void set_querycache_expire(long v) { _querycache_expire = v; }
        const long& devaway_expire() const { return _devaway_expire; }
        void set_devaway_expire(long v) { _devaway_expire = v; }
        const size_t& maxcol() const { return _maxcol; }
        void set_maxcol(size_t v) { _maxcol = v; }

        std::ostream& outstream() const { return *_outstream; }
        void set_outstream(std::ostream *s) { _outstream = s; }
        const std::string& outfile() const { return _outfile; }
        void set_outfile(const std::string& v) { _outfile.assign(v); }
        const std::string& cvsdir() const { return _cvsdir; }
        void set_cvsdir(const std::string& v) { _cvsdir.assign(v); }
        const std::string& herdsxml() const { return _herdsxml; }
        void set_herdsxml(const std::string& v) { _herdsxml.assign(v); }
        const std::string& devawayxml() const { return _devawayxml; }
        void set_devawayxml(const std::string& v) { _devawayxml.assign(v); }
        const std::string& userinfoxml() const { return _userinfoxml; }
        void set_userinfoxml(const std::string& v) { _userinfoxml.assign(v); }
        const std::string& with_herd() const { return _with_herd; }
        void set_with_herd(const std::string& v) { _with_herd.assign(v); }
        const std::string& with_dev() const { return _with_dev; }
        void set_with_dev(const std::string& v) { _with_dev.assign(v); }
        const std::string& localstatedir() const { return _localstatedir; }
        void set_localstatedir(const std::string& v) { _localstatedir.assign(v); }
        const std::string& labelcolor() const { return _labelcolor; }
        void set_labelcolor(const std::string& v) { _labelcolor.assign(v); }
        const std::string& hlcolor() const { return _hlcolor; }
        void set_hlcolor(const std::string& v) { _hlcolor.assign(v); }
        const std::string& metacache_expire() const { return _metacache_expire; }
        void set_metacache_expire(const std::string& v) { _metacache_expire.assign(v); }
        const std::string& highlights() const { return _highlights; }
        void set_highlights(const std::string& v) { _highlights.assign(v); }
        const std::string& locale() const { return _locale; }
        void set_locale(const std::string& v) { _locale.assign(v); }
        const std::string& prompt() const { return _prompt; }
        void set_prompt(const std::string& v) { _prompt.assign(v+" "); }

        const fields_type& fields() const { return _fields; }
        void add_field(const fields_type::value_type v) { _fields.push_back(v); }

        const std::string& iomethod() const { return _iomethod; }
        void set_iomethod(const std::string& v) { _iomethod.assign(v); }

        /* read-only */
        const std::string& portdir() { return _portdir; }
        const std::vector<std::string>& overlays() { return _overlays; }

    private:
        friend Options& GlobalOptions();
        Options();

        bool _verbose;
        bool _quiet;
        bool _debug;
        bool _timer;
        bool _all;
        bool _dev;
        bool _count;
        bool _color;
        bool _overlay;
        bool _eregex;
        bool _regex;
        bool _qa;
        bool _meta;
        bool _metacache;
        bool _querycache;
        bool _devaway;
        bool _fetch;

        int _querycache_max;
        long _querycache_expire;
        long _devaway_expire;
        size_t _maxcol;

        std::ostream *_outstream;
        std::string _outfile;
        std::string _cvsdir;
        std::string _herdsxml;
        std::string _devawayxml;
        std::string _userinfoxml;
        std::string _with_herd;
        std::string _with_dev;
        std::string _localstatedir;
        std::string _wgetopts;
        std::string _labelcolor;
        std::string _hlcolor;
        std::string _metacache_expire;
        std::string _highlights;
        std::string _locale;
        std::string _prompt;

        fields_type _fields;

        std::string _iomethod;

        const std::string& _portdir;
        const std::vector<std::string>& _overlays;
};

inline Options&
GlobalOptions()
{
    static Options o;
    return o;
}

#endif

/* vim: set tw=80 sw=4 fdm=marker et : */
