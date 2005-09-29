/*
 * herdstat -- src/metadata.cc
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

#include "metadata.hh"

bool
metadata_T::dev_exists(const string_type &dev) const
{
    string_type d(dev);
    if (dev.find('@') == string_type::npos)
        d.append("@gentoo.org");
    return this->devs.find(d) != this->devs.end();
}

bool
metadata_T::dev_exists(const util::regex_T &regex) const
{
    herd_type::const_iterator i;
    for (i = this->devs.begin() ; i != this->devs.end() ; ++i)
        if (regex == i->first)
            return true;
    return false;
}

bool
metadata_T::herd_exists(const string_type &herd) const
{
    return std::find(this->herds.begin(), this->herds.end(),
            herd) != this->herds.end();
}

bool
metadata_T::herd_exists(const util::regex_T &regex) const
{
    herds_type::const_iterator i;
    for (i = this->herds.begin() ; i != this->herds.end() ; ++i)
        if (regex == *i)
            return true;
    return false;
}

void
metadata_T::get_pkg_from_path()
{
    assert(not this->path.empty());
    this->pkg = this->path.substr(this->portdir.size() + 1);
    string_type::size_type pos = pkg.find("/metadata.xml");
    if (pos != string_type::npos)
        this->pkg = this->pkg.substr(0, pos);
}

void
metadata_T::display() const
{
    formatter_T output;
    util::vars_T ebuild_vars;
    const bool quiet = optget("quiet", bool);

    /* herds */
    if (not this->is_category and
        (this->herds.empty() or (this->herds.front() == "no-herd")))
        output("Herds(0)", "none");
    else if (not this->herds.empty())
        output(util::sprintf("Herds(%d)", this->herds.size()), this->herds);

    /* devs */
    if (quiet)
    {
        if (this->devs.size() >= 1)
            output("", this->devs.keys());
        else if (not this->is_category)
            output("", "none");
    }
    else
    {
        if (this->devs.size() >= 1)
            output(util::sprintf("Maintainers(%d)", this->devs.size()),
                this->devs.keys().front());
            
        if (this->devs.size() > 1)
        {
            std::vector<herd_type::key_type> dev_keys(this->devs.keys());
            std::vector<herd_type::key_type>::iterator d;
            for (d = ( dev_keys.begin() + 1 ); d != dev_keys.end() ; ++d)
                output("", *d);
        }
        else if (not this->is_category and this->devs.empty())
            output("Maintainers(0)", "none");
    }

    if (not this->is_category)
    {
        std::string ebuild;
        try
        {
            ebuild = portage::ebuild_which(this->portdir, this->pkg);
        }
        catch (const portage::nonexistent_pkg_E)
        {
            ebuild = portage::ebuild_which
                (optget("portage.config", portage::config_T).portdir(), this->pkg);
        }
                
        ebuild_vars.read(ebuild);

        if (quiet and ebuild_vars["HOMEPAGE"].empty())
            ebuild_vars["HOMEPAGE"] = "none";

        if (not ebuild_vars["HOMEPAGE"].empty())
        {
            /* it's possible to have more than one HOMEPAGE */
            if (ebuild_vars["HOMEPAGE"].find("://") != std::string::npos)
            {
                std::vector<std::string> parts = util::split(ebuild_vars["HOMEPAGE"]);

                if (parts.size() >= 1)
                    output("Homepage", parts.front());

                if (parts.size() > 1)
                {
                    std::vector<std::string>::iterator h;
                    for (h = ( parts.begin() + 1) ; h != parts.end() ; ++h)
                        output("", *h);
                }
            }
            else
                output("Homepage", ebuild_vars["HOMEPAGE"]);
        }
    }

    /* long description */
    if (this->longdesc.empty())
    {
        if (not this->is_category)
        {
            if (ebuild_vars["DESCRIPTION"].empty())
                ebuild_vars["DESCRIPTION"] = "none";
                    
            output("Description", ebuild_vars["DESCRIPTION"]);
        }
        else
            output("Description", "none");
    }
    else
        output("Description", util::tidy_whitespace(this->longdesc));
}

/* vim: set tw=80 sw=4 et : */
