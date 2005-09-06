/*
 * herdstat -- portage/metadata.hh
 * $Id: metadata.hh 520 2005-09-05 11:59:58Z ka0ttic $
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

#ifndef _HAVE_METADATA_HH
#define _HAVE_METADATA_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/**
 * @file metadata.hh
 * @brief Defines the metadata class.
 */

#include <string>
#include <herdstat/util/regex.hh>
#include <herdstat/portage/herd.hh>

namespace portage {

    class metadata
    {
        public:
            metadata();
            metadata(const std::string &pkg);

            bool is_category() const;
            const std::string& pkg() const;
            const std::string& longdesc() const;

            void set_category(bool cat);
            void set_pkg(const std::string &pkg);
            void set_longdesc(const std::string &longdesc);

            bool dev_exists(const std::string &dev) const;
            bool dev_exists(const util::regex_T &re) const;
            bool herd_exists(const std::string &herd) const;
            bool herd_exists(const util::regex_T &re) const;

            const Herds& herds() const;
            const Herd& devs() const;

        private:
            friend class metadata_xml;

            std::string _pkg;
            std::string _longdesc;
            bool _cat;
            Herds _herds;
            Herd _devs;
    };

    inline bool metadata::is_category() const { return _cat; }
    inline const std::string& metadata::pkg() const { return _pkg; }
    inline const std::string& metadata::longdesc() const { return _longdesc; }
    inline void metadata::set_category(bool cat) { _cat = cat; }
    inline void metadata::set_pkg(const std::string &pkg) { _pkg.assign(pkg); }
    inline void metadata::set_longdesc(const std::string &longdesc)
    { _longdesc.assign(longdesc); }
    inline const Herds& metadata::herds() const { return _herds; }
    inline const Herd& metadata::devs() const { return _devs; }

} // namespace portage

#endif /* _HAVE_METADATA_HH */

/* vim: set tw=80 sw=4 et : */
