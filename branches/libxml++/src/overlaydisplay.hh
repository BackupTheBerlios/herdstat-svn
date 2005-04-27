/*
 * herdstat -- src/overlaydisplay.hh
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

#ifndef HAVE_OVERLAYDISPLAY_HH
#define HAVE_OVERLAYDISPLAY_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <set>
#include <utility>
#include <sstream>
#include "common.hh"

typedef std::pair<util::string, std::size_t> OverlayCount;

class OverlaySort
{
    public:
        bool operator() (OverlayCount c1, OverlayCount c2)
        { return c1.second < c2.second; }
};

class OverlayDisplay_T
{
    private:
        std::set<OverlayCount, OverlaySort> _oset;

    public:
        typedef std::set<OverlayCount, OverlaySort>::iterator iterator;

        const util::string operator[] (const util::string &s) const
        {
            if (optget("quiet", bool))
                return "";

            util::color_map_T color;
            std::ostringstream os;
            std::size_t n = 0;

            iterator i;
            for (i = this->_oset.begin() ; i != this->_oset.end()  ; ++i)
            {
                if (i->first == s)
                {
                    n = i->second;
                    break;
                }
            }

            assert(n != 0);

            os << color[cyan] << "[" << n << "]" << color[none];
            return os.str();
        }

        void insert(const util::string &s)
        {
            bool found = false;

            /* can't use oset.find() since we only know the second value
             * of the pair before insertion */
            iterator i;
            for (i = this->_oset.begin() ; i != this->_oset.end() ; ++i)
            {
                if (i->first == s)
                {
                    found = true;
                    break;
                }
            }

            if (found)
                return;

            OverlayCount o;
            o.first = s;
            o.second = this->_oset.size() + 1;
            this->_oset.insert(o);
        }

        ~OverlayDisplay_T()
        {
            if (optget("quiet", bool) or this->_oset.empty())
                return;

            std::ostream *stream = optget("outstream", std::ostream *);
            *stream << std::endl << "Portage overlays:" << std::endl;

            iterator i;
            for (i = this->_oset.begin() ; i != this->_oset.end() ; ++i)
                *stream << " " << (*this)[i->first] << " " << i->first
                    << std::endl;
        }
};

#endif

/* vim: set tw=80 sw=4 et : */
