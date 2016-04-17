#*******************************************************************************
# Makefile.am
#-------------------------------------------------------------------------------
##
# \file       	Makefile.am
# \library    	midicvt libraries
# \author     	Chris Ahlstrom
# \date       	2014-05-20
# \update     	2016-04-17
# \version    	$Revision$
# \license    	$XPC_SUITE_GPL_LICENSE$
#
# 		Makefile for the doc/dox subdirectory of the midicvt package.
#
#-------------------------------------------------------------------------------

AUTOMAKE_OPTIONS = foreign dist-zip dist-bzip2
MAINTAINERCLEANFILES = Makefile.in dox-warnings.log

#*******************************************************************************
# EXTRA_DIST
#-------------------------------------------------------------------------------
#
# 	   Some of the sample files are not included in the tar-file, since only
# 	   a developer of an automake project would ever care about them.
#
# 	   The rest are included automatically by the automake process.
#
# xpc_automake.dox xpc_mainpage_generic.dox xpc_mingw.dox xpc_nice_classes.dox
# xpc_philosophy.dox xpc_subversion.dox xpc_suite.dox
#
#------------------------------------------------------------------------------

EXTRA_DIST = make_dox doxy*.cfg

top_srcdir = @top_srcdir@
srcdir = @srcdir@
midicvtdocdir = @midicvtdocdir@

#*******************************************************************************
# Doxygen documentation
#-------------------------------------------------------------------------------
#
#	.PHONY
#
#		The .PHONY directive prevents 'make' from getting confused by an
#		actual file called 'docs' or 'clean', and allows 'make' to continue in
#		spite of errors from doxygen or rm.  A phony target is one that is
#		just a name for some commands to be run on the behest of a specific
#		request.  Declaring a target as phony leaves it out of the search for
#		implicit build rules, enhancing efficiency.
#
# 	DOXYGEN
#
#		This variable is created by the AC_PROG_DOXYGEN macro defined in
#		m4/xpc_doxygen.m4 and used in the configure.ac script.
#
#-------------------------------------------------------------------------------

.PHONY: clean
.PHONY: docs

all: html/*

html/*: docs

# The $(srcdir) component points us to the correct place whether the build
# is in-source or out-of-source.
#
# $(DOXYGEN) $(srcdir)/doxygen.cfg > dox-progress.log 2> dox-errors.log
#
# Can't use pushd/popd because they are bash builtins and are not available,
# except inside a script.
#
# docs: dox-stamp doxy-reference.cfg doxy-userman.cfg

docs: doxy-reference.cfg doxy-userman.cfg
	-echo "We no long automatically rebuild the documentation, it takes too long."
	-echo "Rebuild it by running make_dox three times, with options 'reference',"
	-echo "'clean', and then 'userman'."

#	$(srcdir)/make_dox $(srcdir) reference
#	$(srcdir)/make_dox clean
#	$(srcdir)/make_dox $(srcdir) userman

dox-stamp: $(EXTRA_DIST)
	touch dox-stamp

clean:
	-rm -rf html
	-rm -rf latex
	-rm -f *.log dox-stamp

#******************************************************************************
# Installing documentation
#------------------------------------------------------------------------------
#
# 	   We need to add an install-data-hook to copy the generated
# 	   documentation directories to the destination directory.  The normal
# 	   method doesn't work because /usr/bin/install will only install files,
# 	   and automake doesn't give it the switch needed to install directories.
#
#		cp -r -p $(top_builddir)/doc/dox/html $(midicvtdocdir)
#
#------------------------------------------------------------------------------

install-data-local:
	mkdir -p $(DESTDIR)$(midicvtdocdir)
	cp -r -p $(top_builddir)/doc/*.pdf $(DESTDIR)$(midicvtdocdir)

#*******************************************************************************
# uninstall-hook
#-------------------------------------------------------------------------------

uninstall-hook:
	rm -rf $(DESTDIR)$(midicvtdocdir)

#******************************************************************************
# Makefile.am (dox)
#------------------------------------------------------------------------------
# vim: ts=3 sw=3 noet ft=automake
#------------------------------------------------------------------------------
