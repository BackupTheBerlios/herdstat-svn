/*
 * herdstat -- src/common.cc
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

#include "common.hh"

void
#ifdef UNICODE
debug_msg(const gchar *fmt, ...)
#else /* UNICODE */
debug_msg(const char *fmt, ...)
#endif /* UNICODE */
{
    if (not optget("debug", bool))
	return;
    
    va_list v;
    va_start(v, fmt);
    
    util::string s = util::sprintf(fmt, v);

    /* make ASCII colors visible - TODO: anyway to escape them?
     * simply inserting a '\' before it doesnt work... */
    util::string::size_type pos = s.find("\033");
    if (pos != util::string::npos)
	s.erase(pos, 1);

    *(optget("outstream", std::ostream *)) << "!!! " << s << std::endl;
    va_end(v);
}

/* vim: set tw=80 sw=4 et : */
