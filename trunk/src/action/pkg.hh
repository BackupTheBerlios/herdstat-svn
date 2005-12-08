/*
 * herdstat -- src/action/pkg.hh
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

#ifndef _HAVE__PKG_HH
#define _HAVE__PKG_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <set>
#include <map>
#include "metadata_cache.hh"
#include "action/handler.hh"

class PkgActionHandler : public ActionHandler
{
    public:
        PkgActionHandler();
        virtual ~PkgActionHandler() { }

        virtual const char * const id() const;
        virtual const char * const desc() const;
        virtual const char * const usage() const;

    protected:
        virtual void do_init(Query& query, QueryResults * const results);
        virtual void do_regex(Query& query, QueryResults * const results);
        virtual void do_results(Query& query, QueryResults * const results);
        virtual void do_cleanup(QueryResults * const results);
        virtual gui::Tab *createTab(gui::WidgetFactory *factory);

    private:
        typedef std::map<std::string,
                std::set<herdstat::portage::Metadata> * > matches_type;

        void add_matches(QueryResults * const results);
        bool metadata_matches(const herdstat::portage::Metadata& meta,
                              const std::string& criteria);

        matches_type matches;
        MetadataCache metacache;
        herdstat::util::Regex with;
        herdstat::portage::HerdsXML& herds_xml;
};

#endif /* _HAVE__PKG_HH */

/* vim: set tw=80 sw=4 fdm=marker et : */
