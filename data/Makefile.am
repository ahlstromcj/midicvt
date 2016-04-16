#******************************************************************************
# Makefile.am (midicvt/data)
#------------------------------------------------------------------------------
##
# \file     	Makefile.am
# \library     libmidifilex
# \author      Chris Ahlstrom
# \date        2016-04-14
# \updates     2016-04-15
# \version     $Revision$
# \license     $XPC_SUITE_GPL_LICENSE$
#
#   	This module provides an Automake makefile for the library's
#     ./midicvt/data directory.  These are files we want to copy to
#     /usr/share/midicvt upon installation.
#
#------------------------------------------------------------------------------

#*****************************************************************************
# Packing/cleaning targets
#-----------------------------------------------------------------------------

AUTOMAKE_OPTIONS = foreign dist-zip dist-bzip2
MAINTAINERCLEANFILES = Makefile.in Makefile $(AUX_DIST)

#******************************************************************************
# EXTRA_DIST
#------------------------------------------------------------------------------
#
# 		Note that we have to add the appropriate wildcards to ensure these files
# 		get distributed.
#
#------------------------------------------------------------------------------

EXTRA_DIST = *.ini

#*****************************************************************************
#	Macros
#-----------------------------------------------------------------------------
#
#	This section is recommended by:
#
#		http://olympus.het.brown.edu/cgi-bin/info2www?(gettext)Makefile
#
#-----------------------------------------------------------------------------

PACKAGE = @PACKAGE@
VERSION = @VERSION@

#******************************************************************************
# Install directories
#------------------------------------------------------------------------------
#
# Common prefix for installation directories.  This directory must exist when
# you start the install.
#
# prefix = /usr/local
# datarootdir = $(prefix)/share
# datadir = $(datarootdir)
#
# Where to put the Info files.
#
# infodir = $(datarootdir)/info
#
#------------------------------------------------------------------------------

prefix = @prefix@
datadir = @datadir@
datarootdir = @datarootdir@
pkgdatadir = @pkgdatadir@
midicvtdatadir = @midicvtdatadir@

# datarootdir = $(prefix)/share

#******************************************************************************
# Local project directories
#------------------------------------------------------------------------------

top_srcdir = @top_srcdir@
builddir = @abs_top_builddir@

#------------------------------------------------------------------------------
# install-data files
#------------------------------------------------------------------------------
#
# midicvtdir = $(datarootdir)/midicvt
# midicvt_DATA = GM_PSS-790_Multi.ini
#
#		Instead we use variables defined in configure.ac for now.
#
#------------------------------------------------------------------------------

#******************************************************************************
# Installing documentation and other "data" files.
#------------------------------------------------------------------------------
#
#     We need to add an install-data-hook to copy the generated
#     documentation directories to the destination directory.  The normal
#     method doesn't work because /usr/bin/install will only install files,
#     and automake doesn't give it the switch needed to install directories.
#
#------------------------------------------------------------------------------

install-data-local:
	mkdir -p $(DESTDIR)$(midicvtdatadir)
	cp -r -p $(top_builddir)/data/*.ini $(DESTDIR)$(midicvtdatadir)

#******************************************************************************
# uninstall-hook
#------------------------------------------------------------------------------

uninstall-hook:
	rm -rf $(DESTDIR)$(midicvtdatadir)

#******************************************************************************
# Makefile.am (midicvt/include)
#------------------------------------------------------------------------------
# vim: ts=3 sw=3 noet ft=automake
#------------------------------------------------------------------------------
