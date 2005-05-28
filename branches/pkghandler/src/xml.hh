/*
 * herdstat -- src/xml.hh
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

#ifndef HAVE_XML_HH
#define HAVE_XML_HH 1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <memory>
#include "common.hh"
#include "parsable.hh"
#include "xmlparser.hh"

/*
 * An XML file object
 */

template <class H>
class xml_T : public parsable_T
{
    public:
        typedef H handler_type;
        typedef std::auto_ptr<handler_type> pointer_type;

        xml_T(bool validate = false)
            : parsable_T(), _handler(new handler_type()), _validate(validate) { }
        xml_T(const string_type &p, bool validate = false)
            : parsable_T(p), _handler(new handler_type()), _validate(validate)
        { this->init(); }
        virtual ~xml_T() { }

        handler_type *handler() const { return this->_handler.get(); }

        virtual void parse(const string_type &p = "")
        {
            const util::string file(p.empty() ? this->_path : p);
            XMLParser_T parser(this->_handler.get(), this->_validate);
            this->_timer.start();
            parser.parse(file);
            this->_timer.stop();
        }

    protected:
        virtual void init()
        {
            if (not this->_path.exists())
                throw bad_fileobject_E(this->_path);

            this->parse();
        }

        const pointer_type _handler;    /* content handler */
        const bool _validate;           /* validate XML? */
};

#endif

/* vim: set tw=80 sw=4 et : */
