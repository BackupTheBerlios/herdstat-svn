# Copyright 1999-2005 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

from xml.sax.handler import ContentHandler

class Herds_XML(ContentHandler):
    """Content handler for herds.xml"""

    _inside_herd="No"
    _inside_herd_name="No"
    _inside_herd_email="No"
    _inside_herd_desc="No"
    _inside_maintainer="No"
    _inside_maintainer_name="No"
    _inside_maintainer_email="No"
    _inside_maintainer_role="No"

    # dictionary of herds, containing dicts of dev, containing a list of extra
    # info (name or role).
    # eg. _herds['herd']={'dev1':['name1','role1'], 'dev2':['name2', 'role2']}
    _herds = {}

    # dictionary of herds/descs
    _descs = {}

    # internal use only
    _current_herd = ""
    _current_dev  = ""
    
    def startElement(self, tag, attr):
	if tag == "herd":
	    self._inside_herd="Yes"
	if tag == "name" and self._inside_maintainer == "No":
	    self._inside_herd_name="Yes"
	if tag == "email" and self._inside_maintainer == "No":
	    self._inside_herd_email="Yes"
	if tag == "description" and self._inside_maintainer == "No":
	    self._inside_herd_desc="Yes"
	if tag == "maintainer":
	    self._inside_maintainer="Yes"
	if tag == "email" and self._inside_maintainer == "Yes":
	    self._inside_maintainer_email="Yes"
	if tag == "name" and self._inside_maintainer == "Yes":
	    self._inside_maintainer_name="Yes"
	if tag == "role":
	    self._inside_maintainer_role="Yes"
    
    def endElement(self, tag):
	if tag == "herd":
	    self._inside_herd="No"
	if tag == "name" and self._inside_maintainer == "No":
	    self._inside_herd_name="No"
	if tag == "email" and self._inside_maintainer == "No":
	    self._inside_herd_email="No"
	if tag == "description" and self._inside_maintainer == "No":
	    self._inside_herd_desc="No"
	if tag == "maintainer":
	    self._inside_maintainer="No"
	if tag == "email" and self._inside_maintainer == "Yes":
	    self._inside_maintainer_email="No"
	if tag == "name" and self._inside_maintainer == "Yes":
	    self._inside_maintainer_name="No"
	if tag == "role":
	    self._inside_maintainer_role="No"
    
    def characters(self, contents):
	if self._inside_herd_name == "Yes":
	    self._current_herd = contents
	    self._herds[contents] = {}
	if self._inside_herd_desc == "Yes":
	    self._descs[self._current_herd] = contents
	if self._inside_maintainer_email == "Yes":
	    self._current_dev = contents
	    self._herds[self._current_herd][contents]=[]
	if self._inside_maintainer_name == "Yes":
	    self._herds[self._current_herd][self._current_dev].append(contents)
	if self._inside_maintainer_role == "Yes":
	    self._herds[self._current_herd][self._current_dev].append(contents)
