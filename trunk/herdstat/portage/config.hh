/*
 * herdstat -- herdstat/portage/portage_config.hh
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

#ifndef HAVE_PORTAGE_CONFIG_HH
#define HAVE_PORTAGE_CONFIG_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <herdstat/util/vars.hh>

namespace portage
{
    /**
     * Represents the current portage configuration.  Reads make.conf and
     * make.globals and stores variables in key/value pairs.
     */

    class config : public util::vars
    {
        public:
            /** Default constructor.
             * Opens and reads make.conf and make.globals.
             */
            config();

            /** Determine PORTDIR.
             * @returns A std::string object.
             */
            inline static const std::string& portdir();

            /** Determine PORTDIR_OVERLAY.
             * @returns A vector of std::string objects.
             */
            inline static const std::vector<std::string>& overlays();

        private:
            static bool _init;
            static std::string _portdir;
            static std::vector<std::string> _overlays;
    };

    inline const std::string& config::portdir() { assert(_init); return _portdir; }
    inline const std::vector<std::string>& config::overlays()
    { assert(_init); return _overlays; }

} // namespace portage

/// operator<< for portage::config_T
template<class charT, class traits>
std::basic_ostream<charT, traits> &
operator<< (std::basic_ostream<charT, traits> &stream,
            const portage::config &that)
{
    stream << std::endl;
    portage::config::const_iterator i;
    for (i = that.begin() ; i != that.end() ; ++i)
    {
        std::string s;
        while (s.length() < 20)
            s.append(" ");
        s += i->first + " = " + i->second;
        stream << s << std::endl;
    }
    return stream;
}

#endif

/* vim: set tw=80 sw=4 et : */
