#!/usr/bin/python -O
# Copyright 1999-2005 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

# $Id: herdstat.py 66 2005-03-02 15:44:57Z ka0ttic $

# display statistics about given herd
# Author: Aaron Walker <ka0ttic@gentoo.org>

__author__      = "Aaron Walker"
__email__       = "ka0ttic@gentoo.org"
__productname__ = "herdstat"
__description__ = "display herd statistics"
__version__     = "0.1"

import os
import sys
from xml.sax import make_parser,SAXException

sys.path.insert(0, "/usr/lib/portage/pym")
import portage
from output import *
from portage_herds import Herds_XML
#from portage_metadata import Metadata_XML

settings = portage.config(clone=portage.settings)

herds_url = "http://www.gentoo.org/cgi-bin/viewcvs.cgi/misc/herds.xml?rev=HEAD;cvsroot=gentoo;content-type=text/plain"

def print_usage():
    print "usage: " + __productname__ + " [options] herd(s)\n"
    print " -v,--verbose    Display verbose output"
    print " -p,--package    Show statistics regarding packages for specified herd."
    print "                 Note that this option may take a while."
    print " -h,--help       Display this help message"
    print '\n' + __productname__ + " will check the HERDS environment variable for the"
    print "location of a local herds.xml.  If it is not set, or the file does"
    print "not exist, " + __productname__ + " will try to retrieve it from gentoo.org"

def get_herds_xml(filename):
    """open herds.xml and return file object"""

    # TODO: hook into the new portage transport stuff (pym/transports)

    try:
	if filename[:4] == "http":
	    from urllib import urlopen
	    f = urlopen(filename)
	else:
	    f = open(filename)
    except IOError:
	print red("!!! Failed to open '%s'." % filename)
	return None

    return f

def parse_xml(handler, xml):
    """parse given file object with the given content handler and
       return finished handler"""

    parser = make_parser()
    parser.setContentHandler(handler)

    try:
	parser.parse(xml)
    except SAXException, e:
	print red("!!! Error parsing xml: " + e.getMessage())
	return None

    return handler

def format(prefix, string):
    """Given a prefix and a string, return a string with the appropriate
       amount of whitespace in between"""

    # TODO: use formatters/writers so we don't have to do this crap

    tmpstr = green(prefix)
    if tmpstr != "":
	tmpstr += ":"

    tmpstr = tmpstr.ljust(32) + string
    return tmpstr

def display_dev_data(herd, handler):
    """Display all devs that work on the specified herd."""

    # add some padding for the extra chars present from the coloring
    maxcol = 78 + 20
    if os.environ.has_key('COLUMNS'):
	maxcol = os.environ['COLUMNS'] - 2 + 20

    ndev = len(handler._herds[herd])
    outstr = green("Members(%d)" % ndev)
    if ndev < 10:
	outstr += ":".ljust(3)
    else:
	outstr += ":".ljust(2)
	
    devs = handler._herds[herd].keys()
    devs.sort()

    for dev in devs:
	if verbose:
	    outstr += '\n' + "".ljust(13) + dev + '\n'
	    outstr += "".ljust(13) + handler._herds[herd][dev][0] + '\n'
	    if len(handler._herds[herd][dev]) > 1:
		outstr += "".ljust(13) + handler._herds[herd][dev][1] + '\n'
	else:
	    dev = dev.split('@')[0]

	    # only append if there's enough room
	    if len(outstr) + len(dev) < maxcol:
		outstr += dev + " "
	    # otherwise, it's full; print it, and start a new line with the dev
	    else:
		print outstr
		outstr = "".ljust(13) + dev + " "

    # empty the buffer
    if len(outstr) != 0:
	print outstr

def display_pkg_data(herd, handler):
    """Display all packages that belong in the specified herd."""
    from glob import glob

    npkg = 0
    packages = []
    for category in glob(settings['PORTDIR']+'/*-*'):
	# this isn't the prettiest, but it's the fastest
	out = os.popen('grep -l \<herd\>'+herd+'\</herd\> '+category+'/*/metadata.xml')
	for metadata in out.readlines():
	    npkg += 1
	    p = metadata.split('/')
	    packages.append(p[-3] + '/' + p[-2])
    
    print green("Packages(%d)" % npkg) + ":"

    packages.sort()
    for pkg in packages:
	print "".ljust(13) + pkg

def display_herd_data(herd, handler):
    """Display herd statistics for the specified herd"""

    # does herd exist?
    if not handler._herds.has_key(herd):
	print "Herd '%s' was not found in herds.xml." % herd
	return 1

    print format("Herd", herd)
    print format("Description", handler._descs[herd])
    
    if package:
	display_pkg_data(herd, handler)
    else:
	display_dev_data(herd, handler)

    return 0

def herdstat(herds):
    """display statistics about given herd(s)/dev(s)"""

    # allow user to use local file over URL if we can
    # or even an alternate URL
    if os.environ.has_key('HERDS'):
	herds_xml = get_herds_xml(os.environ['HERDS'])
    else:
	print "HERDS environment variable is not set."
	print "Attempting to retrieve herds.xml from http://www.gentoo.org/."
	herds_xml = get_herds_xml(herds_url)

    if herds_xml == None:
	return 1

    handler = parse_xml(Herds_XML(), herds_xml)
    if handler == None:
	return 1

    herds_xml.close()

    for herd in herds:
	if display_herd_data(herd, handler) != 0:
	    return 1
	# only print seperator newlines if we're not displaying the last one
	if herd != herds[-1]:
	    print '\n'

    return 0

if __name__ == '__main__':
    from getopt import gnu_getopt,GetoptError

    if len(sys.argv) == 1:
	print_usage()
	sys.exit(1)

    try:
	opts,args = gnu_getopt(sys.argv[1:], "hvp", ["help","verbose","package"])
    except GetoptError:
	print_usage()
	sys.exit(1)

    verbose = False
    package = False

    for o, a in opts:
	if o in ("-v", "--verbose"):
	    verbose = True
	if o in ("-h", "--help"):
	    print_usage()
	    sys.exit(0)
	if o in ("-p", "--package"):
	    package = True

    # all non-option args are interpreted as herds
    sys.exit(herdstat(args))
