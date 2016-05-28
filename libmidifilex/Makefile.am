#*****************************************************************************
# Makefile.am (libmidifilex)
#-----------------------------------------------------------------------------
##
# \file          Makefile.am
# \library       libmidifilex
# \author        Chris Ahlstrom
# \date          2013-10-19
# \updates       2014-05-27
# \version       $Revision$
# \license       $MIDICVT_SUITE_GPL_LICENSE$
#
#  	This file is a makefile for the libmidifilex library project.
#
#     This makefile provides the skeleton needed to build the libmidifilex
#     project directory using GNU autotools.
#
#-----------------------------------------------------------------------------

#*****************************************************************************
# Packing targets.
#-----------------------------------------------------------------------------
#
#		Always use Automake in foreign mode (adding foreign to
#		AUTOMAKE_OPTIONS in Makefile.am). Otherwise, it requires too many
#		boilerplate files from the GNU coding standards that aren't useful to
#		us. 
#
#-----------------------------------------------------------------------------

AUTOMAKE_OPTIONS = foreign dist-zip dist-bzip2
MAINTAINERCLEANFILES = Makefile.in Makefile $(AUX_DIST)

#*****************************************************************************
# EXTRA_DIST
#-----------------------------------------------------------------------------
#
#      Extra files in this directory.
#
#      The reference says to use "DISTFILES = ABOUT-NLS".  But vim doesn't
#      highlight that as a keyword, and we don't see reference to it elsewhere
#      on the web.
#
#-----------------------------------------------------------------------------

EXTRA_DIST = README

#*****************************************************************************
#	Packaging.
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
# Installed directories.
#------------------------------------------------------------------------------

prefix = @prefix@
datadir = @datadir@
datarootdir = @datarootdir@
# libdir = @xpclibdir@
# xpclibdir = @xpclibdir@
# xpcdocdir = @xpcdocdir@
# xpcdoxygendir = @xpcdoxygendir@
# xpcincludedir = @xpcincludedir@

#******************************************************************************
# localedir
#------------------------------------------------------------------------------
#
#  'localedir' is the normal system directory for installed localization
#  files.
#
#------------------------------------------------------------------------------

localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

#******************************************************************************
# Local project directories
#------------------------------------------------------------------------------
#
#		top_srcdir    = ../../.. [xpc_suite directory]
#		builddir      = /home/ahlstrom/ca/mls/git/xpc_suite-1.1/debug
#
#------------------------------------------------------------------------------

top_srcdir = @top_srcdir@
builddir = @abs_top_builddir@

#*****************************************************************************
# libtool
#-----------------------------------------------------------------------------

version = $(MIDICVT_API_MAJOR):$(MIDICVT_API_MINOR):$(MIDICVT_API_PATCH)

#*****************************************************************************
# SUBDIRS
#-----------------------------------------------------------------------------

SUBDIRS = include src

#*****************************************************************************
# DIST_SUBDIRS
#-----------------------------------------------------------------------------
#
#      DIST_SUBDIRS is used by targets that need to recurse into /all/
#      directories, even those which have been conditionally left out of the
#      build.
#
#      Precisely, DIST_SUBDIRS is used by:
#
#         -   make dist
#         -   make distclean
#         -   make maintainer-clean.
#
#      All other recursive targets use SUBDIRS.
#
#-----------------------------------------------------------------------------

DIST_SUBDIRS = $(SUBDIRS)

#*****************************************************************************
# Tests
#-----------------------------------------------------------------------------
#
# 		No tests yet included.
#
#-----------------------------------------------------------------------------

#*****************************************************************************
# all-local
#-----------------------------------------------------------------------------

all-local:
	@echo "Top source-directory 'top_srcdir' is $(top_srcdir)"
	@echo "* * * * * All midifile build items completed * * * * *"

#*****************************************************************************
# Makefile.am (libmidifilex top-level)
#-----------------------------------------------------------------------------
# vim: ts=3 sw=3 noet ft=automake
#-----------------------------------------------------------------------------
