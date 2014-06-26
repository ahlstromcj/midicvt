dnl ***************************************************************************
dnl configure.ac (midicvt)
dnl ---------------------------------------------------------------------------
dnl
dnl \file          configure.ac
dnl \library       libmidifilex, libmidipp, midicvt, and midicvtpp
dnl \author        Chris Ahlstrom
dnl \date          2014-04-08
dnl \update        2014-06-02
dnl \version       $Revision$
dnl \license       $MIDICVT_SUITE_GPL_LICENSE$
dnl
dnl   This script is for the midicvt project, a repacking and fixing of the
dnl   midicomp project and the project derived from it, midi2text.
dnl
dnl ---------------------------------------------------------------------------

AC_PREREQ(2.60)
AC_REVISION($Revision: 0.2$)dnl
AC_INIT(midicvt, [0.2], [ahlstromcj@users.sourceforge.net])
AC_CONFIG_AUX_DIR([aux-files])
AC_CONFIG_SRCDIR([libmidifilex/src/midifilex.c])

dnl Do we need the following?  No, but it cleans up a lot of warning messages.

AC_CANONICAL_SYSTEM

dnl See http://www.delorie.com/gnu/docs/automake/automake_24.html

AM_INIT_AUTOMAKE
AC_CONFIG_HEADER([include/config.h])

dnl Prepends "MIDICVT_" to a lot of the macro definitions, for a better
dnl guarantee of avoiding naming conflicts.

AX_PREFIX_CONFIG_H([include/midicvt-config.h], [midicvt])

dnl New for autoconf 2.60, prevents spurious datarootdir warnings.

AC_DEFUN([AC_DATAROOTDIR_CHECKED])

dnl 1. Package version information.  The first item is the name that covers
dnl    a number of libraries, all of whose names start with "midicvt".
dnl    We have to set up a "suite" name; since PACKAGE is defined to be the
dnl    name of the current project, we can't use that value in the package
dnl    configuration files.  It is replaced by MIDICVT_SUITE_NAME, which covers
dnl    all of the library projects.

PACKAGE=midicvt
AC_DEFINE_UNQUOTED(PACKAGE, "$PACKAGE")
AC_SUBST(PACKAGE)

VERSION="0.2.0"
AC_DEFINE_UNQUOTED(VERSION, "$VERSION")
AC_SUBST(VERSION)

MIDICVT_SUITE_NAME="MIDICVT"
AC_SUBST([MIDICVT_SUITE_NAME])

dnl 1.a. Set up version information for this project.
dnl
dnl      This versioning is the libtool version.
dnl      Warning: These values are inserted into libtool's "version-info"
dnl      value, and have to follow libtool rules:
dnl
dnl      Format is 'current:revision:age', where
dnl
dnl      current = Number of the current interface.  If the interface has
dnl      changed, increment this number and set 'revision' to 0.  If the new
dnl      interface has removed elements of the preceding interface, backward
dnl      compatibility has been broken, and you must set 'age' to 0.
dnl
dnl      revision = Implementation number of the most recent interface,
dnl      where 0 is the first.  The runtime loader always uses the library
dnl      with the highest revision.  If you change any source modules,
dnl      increment this number.
dnl
dnl      age = The number of additional interface supported by this library.
dnl      The age must the less than or equal to 'current'.  If the new
dnl      interface is a superset of the preceding interface, 'age' must be
dnl      incremented.

MIDICVT_API_MAJOR="0"
AC_SUBST([MIDICVT_API_MAJOR])

MIDICVT_API_MINOR="1"
AC_SUBST([MIDICVT_API_MINOR])

MIDICVT_API_PATCH="0"
AC_SUBST([MIDICVT_API_PATCH])

MIDICVT_OFFICIAL_VERSION="$MIDICVT_API_MAJOR.$MIDICVT_API_MINOR.$MIDICVT_API_PATCH"

dnl For version 1.1.0, we reset the libtool version information.

MIDICVT_LT_CURRENT="0"
AC_SUBST([MIDICVT_LT_CURRENT])

MIDICVT_LT_REVISION="0"
AC_SUBST([MIDICVT_LT_REVISION])

MIDICVT_LT_AGE="0"
AC_SUBST([MIDICVT_LT_AGE])

MIDICVT_LIBTOOL_VERSION="$MIDICVT_LT_CURRENT.$MIDICVT_LT_REVISION.$MIDICVT_LT_AGE"

dnl 1.b. Set up the version strings.  The primary version string of interest
dnl      is MIDICVT_API_VERSION.

MIDICVT_API_VERSION="$MIDICVT_API_MAJOR.$MIDICVT_API_MINOR"
AC_SUBST([MIDICVT_API_VERSION])

dnl 1.c. Report the version information in the progress output.

AC_MSG_CHECKING(major version)
AC_MSG_RESULT($MIDICVT_API_MAJOR)
AC_MSG_CHECKING(minor version)
AC_MSG_RESULT($MIDICVT_API_MINOR)
AC_MSG_CHECKING(patchlevel)
AC_MSG_RESULT($MIDICVT_API_PATCH)

dnl 1.d. Set up the suite name and current-project name.

MIDICVT_PROJECT_NAME="MIDICVT"
AC_SUBST([MIDICVT_PROJECT_NAME])

dnl 2. Libtool
dnl
dnl Initialize libtool in the default manner, which turns on shared
dnl libraries if available, and enable static libraries if they do not conflict
dnl with shared libraries.  Also sets the shell variable LIBTOOL_DEPS.
dnl
dnl The latest GNU documentation specifies version 2 of libtool, but Debian
dnl testing provides only 1.5 (Debian unstable now provides 2.2).  So we don't
dnl use these new macros, just the older, deprecated macros.
dnl
dnl LT_PREREQ([1.5])
dnl LT_INIT
dnl AC_SUBST([LIBTOOL_DEPS])
dnl
dnl See <http://www.delorie.com/gnu/docs/libtool/libtool_27.html> for the
dnl usage of LIBTOOL_DEPS.
dnl
dnl Uncomment the following if you want more straightforward debugging.  An
dnl alternative is to specify --disable-shared when running 'configure'.
dnl
dnl AC_DISABLE_SHARED

AC_PROG_LIBTOOL

LT_RELEASE="$MIDICVT_API_MAJOR.$MIDICVT_API_MINOR"
AC_SUBST([LT_RELEASE])

LT_CURRENT="$MIDICVT_API_MAJOR"
AC_SUBST([LT_CURRENT])

LT_REVISION="$MIDICVT_API_MINOR"
AC_SUBST([LT_REVISION])

LT_AGE="$MIDICVT_API_PATCH"
AC_SUBST([LT_AGE])

dnl 3. Set up expandable installation directory(ies).
dnl
dnl These directories are for the midicvt library.
dnl
dnl We want all of the header files and library files for the whole suite
dnl to be in the same sub-directory, midicvt-1.1.  This will avoid clutter and put
dnl all our code in one place.
dnl
dnl      /usr/include/midicvt-1.1/midicvt
dnl      /usr/lib/midicvt-1.1
dnl      /usr/man/man1
dnl      /usr/share/doc/midicvt-1.1/doc/dox/html

midicvtincludedir="${includedir}/midicvt-${MIDICVT_API_VERSION}"
AC_SUBST(midicvtincludedir)

midicvtlibdir="${libdir}/midicvt-${MIDICVT_API_VERSION}"
AC_SUBST(midicvtlibdir)

midicvtdocdir="${datadir}/doc/midicvt-${MIDICVT_API_VERSION}"
AC_SUBST(midicvtdocdir)

midicvtdoxygendir="${datadir}/doc/midicvt-${MIDICVT_API_VERSION}"
AC_SUBST(midicvtdoxygendir)

dnl 4. Get external flags.

CFLAGS="${CFLAGS}"

dnl 5. Checks for programs.
dnl
dnl 5.a. Plain C support.
dnl
dnl Many AC macros are not needed, since we use AC_PROG_LIBTOOL, which
dnl defines these for us.
dnl
dnl Check for POSIX

AC_ISC_POSIX

dnl Check for the presence of Doxygen.  Not an error to be missing, but
dnl we don't want to try to build documentation if it is not present.
dnl
dnl See http://www.gnu.org/software/autoconf-archive/ax_prog_doxygen.html

AC_PROG_DOXYGEN

dnl 5.b. gcc support.

AC_GCC_VERSION

dnl C++ support.  Run tests in C++.  Note the order; avoids messy warnings.

AC_LANG_C
AC_LANG_CPLUSPLUS dnl or AC_LANG(C++)
AC_PROG_CXX
AC_PROG_CXXCPP

dnl 5.c. Mingw support.
dnl
dnl Checks for the MingW32 compiler environment. If present, sets shell
dnl variable MINGW32 to 'yes'. If not present, sets MINGW32 to the
dnl empty string.
dnl
dnl Also check to see if the user specified the MingW32 installation
dnl location.  On Debian, the installation is in /usr/i586-mingw32msvc/.
dnl The Mingw versions of the executables start with "i586".  For example,
dnl the compiler is "i586-mingw32msvc-gcc-3.4.5".
dnl
dnl http://www.wxwidgets.org/wiki/index.php/Cross-Compiling_Under_Linux
dnl
dnl is a good place to start learning about building Windows applications.

AC_XPC_MINGW32
AC_ARG_WITH(mingw32,[  --with-mingw32[=DIR]    MingW32 installation location])

dnl Checks for the Cygwin environment. If present, sets shell variable
dnl CYGWIN to 'yes'; if not present, sets CYGWIN to the empty string.

AC_CYGWIN

dnl Clear out the X11 flags for the case where we are cross-compiling
dnl for i586-mingw32 targets, under UNIX.

if test "x$CYGWIN" = "xyes" -o "x$MINGW32" = "xyes" ; then
   X_CFLAGS=""
   X_LIBS=""
fi

dnl 6.0  Top portion of the config.h/midicvt-config.h header files.
dnl      The midicvt-config.h header file has "MIDICVT_" prepended to these
dnl      automatically for us.  Also, we can't use
dnl      #define MIDICVT_API_VERSION "${MIDICVT_API_VERSION}",
dnl      since the macro call in quotes does /not/ get expanded.
dnl
dnl      Note: The AX_PREFIX_CONFIG_H macro called way above prepends
dnl            "MIDICVT_" to these values when it generated midicvt-config.h.

AH_TOP(

#define VERSION_DATE_SHORT "2014-06-02"
#define VERSION "0.2.0"
#define API_VERSION "0.2"

)

dnl 7. Checks for build configuration.
dnl
dnl 7.a. Compiling with debugging, coverage, or profiling options.
dnl
dnl      --enable-debug
dnl      --enable-coverage
dnl      --enable-profiling

AC_XPC_DEBUGGING

dnl 8.  Set up other options in the compiler macros.

APIDEF="-DAPI_VERSION=\"$MIDICVT_API_VERSION\""
WARNFLAGS="-Wall -Wextra -pedantic $WARNINGS"
SPEEDFLAGS="-ffast-math"
COMMONFLAGS="$WARNFLAGS -D_REENTRANT $APIDEF $DBGFLAGS $STACKCHK $NOERRLOG $NOTHISPTR"

dnl Add $WARNINGS_ENABLED to CXXFLAGS if you want to see all warnings, all
dnl the time.

WARNINGS_ENABLED="-Wall"
WARNINGS_DISABLED="-Wno-unused-parameter -Wno-non-virtual-dtor"

dnl CXXFLAGS="$CFLAGS $COMMONFLAGS -Wno-variadic-macros -std=c++11"

CFLAGS="$CFLAGS $COMMONFLAGS"
CXXFLAGS="$CFLAGS $COMMONFLAGS -Wno-variadic-macros"

dnl 10. Checks for header files.

dnl AC_HEADER_STDBOOL

AC_HEADER_TIME
AC_HEADER_STDC

AC_CHECK_HEADERS([stdarg.h stdio.h stddef.h])
AC_CHECK_HEADERS([stdlib.h string.h])
AC_CHECK_HEADERS([limits.h ctype.h sys/time.h time.h errno.h])
AC_CHECK_HEADERS([errno.h sys/sysctl.h])
AC_CHECK_HEADERS([math.h])

AC_CHECK_TYPES([errno_t], [], [], [[#include <errno.h>]])

dnl Checks for typedefs, structures, and compiler characteristics.

AC_C_CONST
AC_TYPE_SIZE_T
AC_STRUCT_TM

dnl Flex support
dnl
dnl Sometimes Flex is installed as Lex, e.g., NetBSD.
dnl Force the use of 'missing' to wrap Flex invocations.
dnl Perform all the tests Automake and Autoconf need.
dnl
dnl AC_CHECK_PROG([FLEX], [flex lex], [flex])
dnl AM_MISSING_PROG([LEX], [$FLEX])
dnl AM_PROG_LEX
dnl
dnl AX_PROG_FLEX([echo "flex"],[echo "no flex"])

dnl Checks for library functions.

AC_FUNC_MALLOC
AC_FUNC_SELECT_ARGTYPES
AC_CHECK_FUNCS([strerror])

dnl 11. Checks for internationalization macros (i18n).
dnl
dnl These checks are derived from the GNU "hello world" project at
dnl
dnl      http://ftp.gnu.org/gnu/hello/

AC_CHECK_HEADERS([libintl.h locale.h])

dnl No internationalization support at present.
dnl
dnl AM_PO_SUBDIRS
dnl AM_GNU_GETTEXT([external])
dnl AM_GNU_GETTEXT_VERSION([0.18.1])

AH_BOTTOM(
   /* Bottom of header config */
)

dnl ./gmidicvt

AC_CONFIG_FILES([
 Makefile
 m4/Makefile
 libmidifilex/Makefile
 libmidifilex/include/Makefile
 libmidifilex/src/Makefile
 libmidipp/Makefile
 libmidipp/include/Makefile
 libmidipp/src/Makefile
 midicvt/Makefile
 midicvtpp/Makefile
 doc/dox/Makefile
 ])

AC_OUTPUT

cat << E_O_F

Then do 'make' to compile the midicvt-related projects.
Do 'make check' or 'make test' to run the tests of the projects.
Do 'make install' to install the midicvt application.
Do 'make dist', etc. to create gzip and other archives.

E_O_F

dnl ***************************************************************************
dnl configure.ac (midicvt)
dnl ---------------------------------------------------------------------------
dnl Local Variables:
dnl End:
dnl ---------------------------------------------------------------------------
dnl vim: ts=3 sw=3 et ft=config
dnl ---------------------------------------------------------------------------
