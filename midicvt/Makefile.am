#******************************************************************************
# Makefile.am (midicvt)
#------------------------------------------------------------------------------
##
# \file       	Makefile.am
# \library    	midicvt application
# \author     	Chris Ahlstrom
# \date       	2014-04-08
# \update      2014-04-19
# \version    	$Revision$
# \license    	$XPC_SUITE_GPL_LICENSE$
#
# 		This module provides an Automake makefile for the midicvt application.
#
#------------------------------------------------------------------------------

#*****************************************************************************
# Packing/cleaning targets
#-----------------------------------------------------------------------------

AUTOMAKE_OPTIONS = foreign dist-zip dist-bzip2
MAINTAINERCLEANFILES = Makefile.in Makefile $(AUX_DIST)

#******************************************************************************
# CLEANFILES
#------------------------------------------------------------------------------

CLEANFILES = *.gc*

#******************************************************************************
#  EXTRA_DIST
#------------------------------------------------------------------------------
#
#  getopt_test.c is not ready and is not included at this time.
#	$(TESTS) is not included because it is derived files.
#
#------------------------------------------------------------------------------

# EXTRA_DIST = dl_leaks.supp make-tests README

#******************************************************************************
# Items from configure.ac
#-------------------------------------------------------------------------------
#
#  XPCCUT_LIBS is defined by the PKG_CHECK_MODULES macro.  The xpccut
#  library must be installed officially in order to be used to build the
#  unit-test application of the xpc library.
#
#-------------------------------------------------------------------------------

PACKAGE = @PACKAGE@
VERSION = @VERSION@

#******************************************************************************
# Install directories
#------------------------------------------------------------------------------
#
# 	Not needed, yet, since we won't be installing the app for awhile.
#
#------------------------------------------------------------------------------

prefix = @prefix@
libdir = @xpclibdir@
datadir = @datadir@
datarootdir = @datarootdir@
midicvtincludedir = @midicvtincludedir@
midicvtlibdir = @midicvtlibdir@

#******************************************************************************
# localedir
#------------------------------------------------------------------------------
#
# 	'localedir' is the normal system directory for installed localization
#  files.
#
#------------------------------------------------------------------------------

localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

#******************************************************************************
# Local project directories
#------------------------------------------------------------------------------

top_srcdir = @top_srcdir@
builddir = @abs_top_builddir@

libmidifiledir = $(builddir)/libmidifilex/src/.libs

#******************************************************************************
# AM_CPPFLAGS [formerly "INCLUDES"]
#------------------------------------------------------------------------------
#
# 	'AM_CPPFLAGS' is the set of directories needed to access all of the
# 	XPC-suite unit-test library header files used in this project.
#
#		Defines where to look for header files.
#
#			-	If we build from the xpc_suite root directory, then $top_srcdir
#           is "../.."
#			-	If we build from the xpc directory, then it is "..".
#			-	If we enter a directory such as "debug", and do "../configure",
#				then it is "../../.."
#
#		"../include" is where the xpc header files are located, and it must
#		be defined even if $top_srcdir is "../..".  So we need two entries,
#		to handle the two possible build situations.
#
#		By convention, we include the "xpc" subdirectory in the C/C++ #include
#		directives, so it is not needed here.
#
#------------------------------------------------------------------------------

AM_CPPFLAGS = -I$(top_srcdir)/libmidifilex/include

#******************************************************************************
# libmath
#------------------------------------------------------------------------------
#
# 		One day, we got errors about sqrt() undefined, which we fixed by
# 		adding -lm.  Then one day we got errors about various items in
# 		sys/stat.h being multiply-defined, and it turned out to be the -lm.
#
# 		We make it (an empty) define for how to handle it more easily.
#
#------------------------------------------------------------------------------

libmath = -lm

#****************************************************************************
# Project-specific library files
#----------------------------------------------------------------------------
#
#	These files are the ones built in the source tree, not the installed
#	ones.
#
#  Sometimes one has to change the order of the libraries in this list.
#
#----------------------------------------------------------------------------

libraries = $(libmath) -L$(libmidifiledir) -lmidifilex

#****************************************************************************
# Project-specific dependency files
#----------------------------------------------------------------------------
#
#  Provdies the specific list of dependencies, to assure that the make
#  detects all changes, if they are available.
#
#----------------------------------------------------------------------------

dependencies = $(libmidifiledir)/libmidifilex.la

#******************************************************************************
# The programs to build
#------------------------------------------------------------------------------

bin_PROGRAMS = midicvt

#******************************************************************************
# midicvt
#----------------------------------------------------------------------------

midicvt_SOURCES = midicvt_main.c
midicvt_LDADD = -lpthread -ldl $(libraries)
midicvt_DEPENDENCIES = $(dependencies)

#******************************************************************************
# Testing
#------------------------------------------------------------------------------
#
# 	   http://www.gnu.org/software/hello/manual/automake/Simple-Tests.html
#
#------------------------------------------------------------------------------

testsubdir = test-results
TESTS_ENVIRONMENT =
TESTS = test_script

test: check

#******************************************************************************
#  distclean
#------------------------------------------------------------------------------

distclean-local:
	-rm -rf $(testsubdir)

#******************************************************************************
# Makefile.am (midicvt)
#------------------------------------------------------------------------------
# Local Variables:
# End:
#------------------------------------------------------------------------------
# 	vim: ts=3 sw=3 ft=automake
#------------------------------------------------------------------------------
