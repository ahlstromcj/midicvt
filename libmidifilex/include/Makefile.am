#******************************************************************************
# Makefile.am (midicvt/include)
#------------------------------------------------------------------------------
##
# \file     	Makefile.am
# \library     libmidifilex
# \author      Chris Ahlstrom
# \date        2014-04-08
# \updates     2016-05-28
# \version     $Revision$
# \license     $XPC_SUITE_GPL_LICENSE$
#
#   	This module provides an Automake makefile for the library's
#     ./include/midicvt directory.
#
# \note
#   	The normal and re-named configuration header files created are:
#
#     	- 	config.h
#        -  midicvt-config.h
#
#		All project share the same file, which is now locating in
#		$(top_srcdir)/include/midicvt.
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
# 		Note that we have to add "*.h" to ensure these files get distributed.
#
#------------------------------------------------------------------------------

EXTRA_DIST = *.h

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

MIDICVT_API_MAJOR = @MIDICVT_API_MAJOR@
MIDICVT_API_MINOR = @MIDICVT_API_MINOR@
MIDICVT_API_PATCH = @MIDICVT_API_PATCH@
MIDICVT_API_VERSION = @MIDICVT_API_VERSION@

MIDICVT_LT_CURRENT = @MIDICVT_LT_CURRENT@
MIDICVT_LT_REVISION = @MIDICVT_LT_REVISION@
MIDICVT_LT_AGE = @MIDICVT_LT_AGE@

#******************************************************************************
# Local project directories
#------------------------------------------------------------------------------

top_srcdir = @top_srcdir@
builddir = @abs_top_builddir@

#******************************************************************************
# Install directories
#------------------------------------------------------------------------------

prefix = @prefix@
includedir = @midicvtincludedir@
libdir = @midicvtlibdir@
datadir = @datadir@
datarootdir = @datarootdir@
midicvtlibdir = @midicvtlibdir@
midicvtdocdir = @midicvtdocdir@
midicvtdoxygendir = @midicvtdoxygendir@
midicvtincludedir = @midicvtincludedir@

#******************************************************************************
# Locale directory
#------------------------------------------------------------------------------
#
#     Not yet used.
#
#------------------------------------------------------------------------------

localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

#******************************************************************************
# Local project directories
#------------------------------------------------------------------------------

top_srcdir = @top_srcdir@
builddir = @abs_top_builddir@

#******************************************************************************
# Include directory
#------------------------------------------------------------------------------
#
# 		This define is necessary to cause the proper subdirectory to be made
# 		during installation.  'midicvtincludedir' is defined in the configure.ac
# 		script.
#
#  	Without this value, the header files are stored in
#  	"$(prefix)/include/midicvt-1.0" instead of
#  	"$(prefix)/include/midicvt-1.0/midicvt".
# 		pkgincludedir=$(midicvtincludedir)/midicvt
#
#------------------------------------------------------------------------------

#******************************************************************************
# Header files
#------------------------------------------------------------------------------

pkginclude_HEADERS = \
 VERSION.h \
 midicvt_base.h \
 midicvt_cc.h \
 midicvt_globals.h \
 midicvt_helpers.h \
 midicvt_macros.h \
 midicvt_m2m.h \
 midifilex.h \
 t2mf.h

#******************************************************************************
# uninstall-hook
#------------------------------------------------------------------------------
#
#     We'd like to remove /usr/local/include/midicvt-1.0/midicvt if it is
#     empty.  However, we don't have a good way to do it yet.
#
#------------------------------------------------------------------------------

uninstall-hook:
	@echo "Note:  you may want to remove $(pkgincludedir) manually"

#******************************************************************************
# Makefile.am (midicvt/include)
#------------------------------------------------------------------------------
# vim: ts=3 sw=3 noet ft=automake
#------------------------------------------------------------------------------
