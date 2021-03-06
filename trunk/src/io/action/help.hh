/*
 * herdstat -- src/io/action/help.hh
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

#ifndef _HAVE_ACTION_HELP_HH
#define _HAVE_ACTION_HELP_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "io/action/handler.hh"

/**
 * @class HelpIOActionHandler
 * @brief IOActionHandler for displaying help.
 */

class HelpIOActionHandler : public IOActionHandler
{
    public:
        virtual ~HelpIOActionHandler() { }
        
        virtual bool allow_empty_query() const;
        virtual const char * const id() const;
        virtual const char * const desc() const;
        virtual void generate_completions(std::vector<std::string> *) const;        

    protected:
        virtual void do_results(Query& query, QueryResults * const results);
};

#endif /* _HAVE_ACTION_HELP_HH */

/* vim: set tw=80 sw=4 et : */
