#******************************************************************************
# Makefile.am (libmidipp/include)
#------------------------------------------------------------------------------
##
# \file     	Makefile.am
# \library     libmidipp
# \author      Chris Ahlstrom
# \date        2014-04-22
# \updates     2014-05-02
# \version     $Revision$
# \license     $XPC_SUITE_GPL_LICENSE$
#
#   	This module provides an Automake makefile for the XPC library's
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

EXTRA_DIST = *.hpp

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
# Install directories
#------------------------------------------------------------------------------

prefix = @prefix@
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
# Header files
#------------------------------------------------------------------------------

pkginclude_HEADERS = \
 csvarray.hpp \
 initree.hpp \
 iniwriting.hpp \
 midimapper.hpp \
 stringmap.hpp

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
# Debugging targets
#------------------------------------------------------------------------------

show:
	@echo "Install directories:"
	@echo
	@echo "prefix        = $(prefix)"
	@echo "datadir       = $(datadir)"
	@echo "datarootdir   = $(datarootdir)"
	@echo "libdir        = $(libdir)"
	@echo "midicvtlibdir     = $(midicvtlibdir)"
	@echo "midicvtdocdir     = $(midicvtdocdir)"
	@echo "midicvtdoxygendir = $(midicvtdoxygendir)"
	@echo "midicvtincludedir = $(midicvtincludedir)"
	@echo "localedir     = $(localedir)"
	@echo
	@echo "Local directories:"
	@echo
	@echo "top_srcdir    = $(top_srcdir) [XPC Basic root directory]"
	@echo "srcdir    		= $(srcdir)"
	@echo "top_builddir  = $(top_builddir)"
	@echo "builddir      = $(builddir)"

#******************************************************************************
# Makefile.am (libmidipp/include)
#------------------------------------------------------------------------------
# Local Variables:
# End:
#------------------------------------------------------------------------------
# vim: ts=3 sw=3 noet ft=automake
#------------------------------------------------------------------------------
