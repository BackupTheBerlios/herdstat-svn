/*
 * herdstat -- action/handler.hh
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

#ifndef _HAVE_ACTION_HANDLER_HH
#define _HAVE_ACTION_HANDLER_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <map>
#include <herdstat/util/misc.hh>
#include <herdstat/util/timer.hh>
#include <herdstat/util/regex.hh>
#include <herdstat/util/progress/spinner.hh>
#include <herdstat/portage/package_finder.hh>

#include "options.hh"
#include "package_cache.hh"
#include "query.hh"
#include "query_results.hh"
#include "io/gui/widget_factory.hh"

class ActionHandler
{
    public:
        ActionHandler();
        virtual ~ActionHandler();

        /// perform action
        void operator()(Query& query, QueryResults * const results);

        /** this handler allows using data found in ${PWD} to be used as the
          * query?
          */
        virtual bool allow_pwd_query() const;
        /// Generate query based on data in ${PWD}.
        virtual void handle_pwd_query(Query * const query,
                                      QueryResults * const results);
        /// this handler allows empty query objects to be passed?
        virtual bool allow_empty_query() const;
        /// action identifier string
        virtual const char * const id() const = 0;
        /// action description string
        virtual const char * const desc() const = 0;
        /// action usage string
        virtual const char * const usage() const;

    protected:
        virtual void do_init(Query& query, QueryResults * const results);
        virtual void do_all(Query& query, QueryResults * const results);
        virtual void do_regex(Query& query, QueryResults * const results) = 0;
        virtual void do_results(Query& query, QueryResults * const results) = 0;
        virtual void do_cleanup(QueryResults * const results);

        /* Called by GuiIOHandler::operator() when
         * filling the TabBar. */
        friend class GuiIOHandler;
        virtual gui::Tab *createTab(gui::WidgetFactory *factory) = 0;

        /* result size (not necessarily same size as results
         * object passed to operator()). */
        int& size() { return _size; }
        /* did the handler err at least once? */
        bool& error() { return _err; }

        inline void increment_spinner();
        inline void stop_spinner();
        inline void start_spinner(unsigned total,
                                  const std::string& title = "");
        inline herdstat::util::ProgressMeter *spinner() const
        { return _spinner; } 
        inline void set_spinner(herdstat::util::ProgressMeter *p)
        { _spinner = p; }

        Options& options;
        herdstat::util::ColorMap& color;
        herdstat::util::Regex regexp;

    private:
        bool _err;
        int _size;
        herdstat::util::ProgressMeter *_spinner;
};

inline void
ActionHandler::increment_spinner()
{
    if (_spinner)
        ++*_spinner;
}

inline void
ActionHandler::stop_spinner()
{
    if (_spinner)
    {
        delete _spinner;
        _spinner = NULL;
    }
//        _spinner->stop();
}

inline void
ActionHandler::start_spinner(unsigned total, const std::string& title)
{
    if (not _spinner)
        _spinner = new herdstat::util::Spinner();
    if (not _spinner->started())
        _spinner->start(total, title);
}

class PortageSearchActionHandler : public ActionHandler
{
    public:
        virtual ~PortageSearchActionHandler();

        virtual void handle_pwd_query(Query * const query,
                                      QueryResults * const results);

    protected:
        PortageSearchActionHandler();

        virtual void do_init(Query& query, QueryResults * const results);
        virtual void do_regex(Query& query, QueryResults * const results);
        virtual void do_cleanup(QueryResults * const results);

        inline void remove_overlay_packages();
        inline bool is_ambiguous(const std::vector<herdstat::portage::Package>& pkgs);

        inline herdstat::portage::PackageFinder& find();
        inline bool pwd_mode() const { return _pwd; }
        inline void set_pwd_mode(bool v) { _pwd = v; }

        std::vector<herdstat::portage::Package> matches;

    private:
        herdstat::portage::PackageFinder *_find;
        bool _pwd;
};

inline void
PortageSearchActionHandler::remove_overlay_packages()
{
    matches.erase(std::remove_if(matches.begin(), matches.end(),
        std::mem_fun_ref(&herdstat::portage::Package::in_overlay)), matches.end());
}

inline bool
PortageSearchActionHandler::is_ambiguous
    (const std::vector<herdstat::portage::Package>& pkgs)
{
    return ((pkgs.size() > 1) and not
        (herdstat::util::all_equal(pkgs.begin(), pkgs.end(),
            std::mem_fun_ref(&herdstat::portage::Package::full))));
}

inline herdstat::portage::PackageFinder&
PortageSearchActionHandler::find()
{
    if (not _find)
        _find = new herdstat::portage::PackageFinder(GlobalPkgCache(spinner()));
    return *_find;
}

#endif /* _HAVE_ACTION_HANDLER_HH */

/* vim: set tw=80 sw=4 fdm=marker et : */
