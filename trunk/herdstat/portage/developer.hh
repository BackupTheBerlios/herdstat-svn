/*
 * herdstat -- portage/developer.hh
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

#ifndef _HAVE_DEVELOPER_HH
#define _HAVE_DEVELOPER_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/**
 * @file developer.hh
 * @brief Defines the developer class.
 */

#include <string>

namespace portage {

    /**
     * Represents a Gentoo developer.
     */

    class developer
    {
        public:
            /// Default constructor.
            developer() { }

            /** Constructor.
             * @param email email address.
             * @param name  name.
             */
            developer(const std::string &email,
                      const std::string &name = "");

            /** Implicit conversion to std::string.
             * @returns Developer user name.
             */
            operator std::string() const;

            const std::string& user() const;
            const std::string& email() const;
            const std::string& name() const;
            const std::string& pgpkey() const;
            const std::string& joined() const;
            const std::string& birth() const;
            const std::string& status() const;
            const std::string& role() const;
            const std::string& awaymsg() const;
            bool is_away() const;

            void set_user(const std::string &user);
            void set_email(const std::string &email);
            void set_name(const std::string &name);
            void set_pgpkey(const std::string &pgpkey);
            void set_joined(const std::string &date);
            void set_birth(const std::string &date);
            void set_status(const std::string &status);
            void set_role(const std::string &role);
            void set_awaymsg(const std::string &msg);
            void set_away(const bool away);

        private:
            std::string _user;
            std::string _email;
            std::string _name;
            std::string _pgpkey;
            std::string _joined;
            std::string _birth;
            std::string _status;
            std::string _role;
            std::string _awaymsg;
            bool _away;
    };

    inline developer::operator std::string() const { return _user; }
    inline const std::string& developer::user() const { return _user; }
    inline const std::string& developer::email() const { return _email; }
    inline const std::string& developer::name() const { return _name; }
    inline const std::string& developer::pgpkey() const { return _pgpkey; }
    inline const std::string& developer::joined() const { return _joined; }
    inline const std::string& developer::birth() const { return _birth; }
    inline const std::string& developer::status() const { return _status; }
    inline const std::string& developer::role() const { return _role; }
    inline const std::string& developer::awaymsg() const { return _awaymsg; }
    inline bool developer::is_away() const { return _away; }
    inline void developer::set_user(const std::string &user)
    { _user.assign(user); }
    inline void developer::set_email(const std::string &email)
    { _email.assign(email); }
    inline void developer::set_name(const std::string &name)
    { _name.assign(name); }
    inline void developer::set_pgpkey(const std::string &pgpkey)
    { _pgpkey.assign(pgpkey); }
    inline void developer::set_joined(const std::string &date)
    { _joined.assign(date); }
    inline void developer::set_birth(const std::string &date)
    { _birth.assign(date); }
    inline void developer::set_status(const std::string &status)
    { _status.assign(status); }
    inline void developer::set_role(const std::string &role)
    { _role.assign(role); }
    inline void developer::set_awaymsg(const std::string &msg)
    { _awaymsg.assign(msg); }
    inline void developer::set_away(const bool away) { _away = away; }

} // namespace portage

#endif /* _HAVE_DEVELOPER_HH */

/* vim: set tw=80 sw=4 et : */
