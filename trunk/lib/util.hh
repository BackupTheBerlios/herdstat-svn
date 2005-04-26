/*
 * herdstat -- lib/util.hh
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

#ifndef HAVE_UTIL_HH
#define HAVE_UTIL_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <cstdarg>
#include <cstdlib>
#include <cstddef>
#include <cstdio>

#ifndef PATH_MAX
# define PATH_MAX   4096
#endif

#include "misc.hh"
#include "file.hh"
#include "vars.hh"
#include "regex.hh"
#include "string.hh"
#include "timer.hh"
#include "progress.hh"
#include "util_exceptions.hh"
#include "portage_exceptions.hh"
#include "portage_misc.hh"
#include "portage_config.hh"
#include "portage_version.hh"

#endif
