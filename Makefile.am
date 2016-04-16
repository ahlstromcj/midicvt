#*****************************************************************************
# Makefile.am (midicvt)
#-----------------------------------------------------------------------------
##
# \file        Makefile.am
# \library     libmidifilex, libmidipp, midicvt, and midicvtpp
# \author      Chris Ahlstrom
# \date        2014-04-08
# \updates     2016-04-15
# \version     $Revision$
# \license     $MIDICVT_SUITE_GPL_LICENSE$
#
#     This is the top-level project makefile for the midicvt projects.
#
#     This makefile provides the skeleton needed to build the library
#     and application directories using GNU autotools.
#
# \note
#     We currently leave the doc/dox directory out of SUBDIRS, to avoid
#     building the PDF files over and over.
#
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Packing targets.
#-----------------------------------------------------------------------------

AUTOMAKE_OPTIONS = foreign dist-zip dist-bzip2
MAINTAINERCLEANFILES = Makefile.in Makefile $(AUX_DIST)

#*****************************************************************************
# Extra files in the top-level directory
#-----------------------------------------------------------------------------

EXTRA_DIST = bootstrap build pack README TODO VERSION

#*****************************************************************************
# Packaging.
#-----------------------------------------------------------------------------
#
#     This section is recommended for the top-level Makefile.am by the
#     reference.
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

#*****************************************************************************
# Installed directories.
#-----------------------------------------------------------------------------
#
# 		The 'libdir' define is necessary to cause the proper subdirectory to
# 		be made during installation.  'midicvtlibdir' is defined in the
# 		configure.ac script.  The 'libdir' define is needed for work with
# 		libtool.  Not sure about 'pkglibdr'.
#
# 				pkglibdir=$(midicvtlibdir)
#
#	Directories and macros:
#
#		prefix        = /usr/local
#		libdir        = /usr/local/lib/midicvt
#		datadir       = /usr/local/share
#		datarootdir   = /usr/local/share
#		midicvtlibdir     = /usr/local/lib/midicvt
#		midicvtdocdir     = /usr/local/share/doc/midicvt-1.1
#		midicvtdoxygendir = doc
#		midicvtincludedir = /usr/local/include/midicvt-1.1
#		localedir     = /usr/local/share/locale
#
#  'localedir' is the normal system directory for installed localization
#  files.
#
#-----------------------------------------------------------------------------

prefix = @prefix@
libdir = @midicvtlibdir@
datadir = @datadir@
datarootdir = @datarootdir@
midicvtdocdir = @midicvtdocdir@
midicvtdoxygendir = @midicvtdoxygendir@
midicvtincludedir = @midicvtincludedir@
midicvtlibdir = @midicvtlibdir@
localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

#******************************************************************************
# Local project directories
#------------------------------------------------------------------------------
#
#		top_srcdir    = ../../.. [XPC Basic directory]
#		builddir      = /home/ahlstrom/ca/mls/git/XPC Basic-1.1/debug
#
#------------------------------------------------------------------------------

top_srcdir = @top_srcdir@
builddir = @abs_top_builddir@

#******************************************************************************
# aclocal support
#------------------------------------------------------------------------------

ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

#*****************************************************************************
# libtool
#-----------------------------------------------------------------------------
#	${SHELL} $(top_srcdir)/config.status --recheck
#-----------------------------------------------------------------------------

LIBTOOL_DEPS = @LIBTOOL_DEPS@

libtool: $(LIBTOOL_DEPS)
	$(top_srcdir)/config.status --recheck

#*****************************************************************************
# SUBDIRS
#-----------------------------------------------------------------------------
#
# SUBDIRS = man doc pkgconfig po
#
#  Note that doc/dox is not commented out.  If you do not want to Build the
#  documentation, then comment out the "doc/dox" specification.  You can still
#  make the documentation manually by calling the make_dox script, in that
#  case.
#
#-----------------------------------------------------------------------------

SUBDIRS = m4 libmidifilex midicvt libmidipp midicvtpp data doc/dox

#*****************************************************************************
# DIST_SUBDIRS
#-----------------------------------------------------------------------------

DIST_SUBDIRS = $(SUBDIRS)

#*****************************************************************************
# Tests
#-----------------------------------------------------------------------------
#
#      Uncomment these items only if your project does not support "make
#      check" or "make test".
#
# .PHONY: test check
#
# test: check
#
# check:
#   @echo "'make check' is not implemented in the midicvt library."
#
#-----------------------------------------------------------------------------

#*****************************************************************************
# all-local
#-----------------------------------------------------------------------------

all-local:
	@echo "Top source-directory 'top_srcdir' is $(top_srcdir)"
	@echo "* * * * * All build items completed * * * * *"

#******************************************************************************
# Debugging targets
#------------------------------------------------------------------------------

show:
	@echo "Install directories:"
	@echo
	@echo "prefix           = $(prefix)"
	@echo "datadir          = $(datadir)"
	@echo "datarootdir      = $(datarootdir)"
	@echo "libdir           = $(libdir)"
	@echo "midicvtlibdir      = $(midicvtlibdir)"
	@echo "midicvtdocdir      = $(midicvtdocdir)"
	@echo "midicvtdoxygendir  = $(midicvtdoxygendir)"
	@echo "midicvtincludedir  = $(midicvtincludedir)"
	@echo "localedir        = $(localedir)"
	@echo
	@echo "Local directories:"
	@echo
	@echo "top_srcdir       = $(top_srcdir) [project root directory]"
	@echo "srcdir           = $(srcdir)"
	@echo "top_builddir     = $(top_builddir)"
	@echo "builddir         = $(builddir)"

showver:
	@echo "PACKAGE            = $(PACKAGE)"
	@echo "VERSION            = $(VERSION)"
	@echo "MIDICVT_API_MAJOR    = $(MIDICVT_API_MAJOR)"
	@echo "MIDICVT_API_MINOR    = $(MIDICVT_API_MINOR)"
	@echo "MIDICVT_API_PATCH    = $(MIDICVT_API_PATCH)"
	@echo "MIDICVT_API_VERSION  = $(MIDICVT_API_VERSION)"
	@echo "MIDICVT_LT_CURRENT   = $(MIDICVT_LT_CURRENT)"
	@echo "MIDICVT_LT_REVISION  = $(MIDICVT_LT_REVISION)"
	@echo "MIDICVT_LT_AGE       = $(MIDICVT_LT_AGE)"

#****************************************************************************
# Makefile.am (midicvt top-level)
#----------------------------------------------------------------------------
# Local Variables:
# End:
#------------------------------------------------------------------------------
# vim: ts=3 sw=3 ft=automake
#----------------------------------------------------------------------------
