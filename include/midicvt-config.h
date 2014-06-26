#ifndef _INCLUDE_MIDICVT_CONFIG_H
#define _INCLUDE_MIDICVT_CONFIG_H 1
 
/* include/midicvt-config.h. Generated automatically at end of configure. */
/* include/config.h.  Generated from config.h.in by configure.  */
/* include/config.h.in.  Generated from configure.ac by autoheader.  */

#ifndef MIDICVT_VERSION_DATE_SHORT
#define MIDICVT_VERSION_DATE_SHORT "2014-06-02"
#endif
#ifndef MIDICVT_VERSION
#define MIDICVT_VERSION "0.2.0"
#endif
#ifndef MIDICVT_API_VERSION
#define MIDICVT_API_VERSION "0.2"
#endif



/* Define COVFLAGS=-fprofile-arcs -ftest-coverage if coverage support is
   wanted. */
#ifndef MIDICVT_COVFLAGS
#define MIDICVT_COVFLAGS 
#endif

/* Define on cygwin */
/* #undef CYGWIN */

/* Define DBGFLAGS=-g -O0 -DDEBUG -NWIN32 -fno-inline if debug support is
   wanted. */
#ifndef MIDICVT_DBGFLAGS
#define MIDICVT_DBGFLAGS -ggdb -O0 -DDEBUG -D_DEBUG -DNWIN32 -fno-inline
#endif

/* Define to 1 if you have the <ctype.h> header file. */
#ifndef MIDICVT_HAVE_CTYPE_H
#define MIDICVT_HAVE_CTYPE_H 1
#endif

/* Define to 1 if you have the <dlfcn.h> header file. */
#ifndef MIDICVT_HAVE_DLFCN_H
#define MIDICVT_HAVE_DLFCN_H 1
#endif

/* Define to 1 if you have the <errno.h> header file. */
#ifndef MIDICVT_HAVE_ERRNO_H
#define MIDICVT_HAVE_ERRNO_H 1
#endif

/* Define to 1 if the system has the type `errno_t'. */
/* #undef HAVE_ERRNO_T */

/* Define to 1 if you have the <inttypes.h> header file. */
#ifndef MIDICVT_HAVE_INTTYPES_H
#define MIDICVT_HAVE_INTTYPES_H 1
#endif

/* Define to 1 if you have the <libintl.h> header file. */
#ifndef MIDICVT_HAVE_LIBINTL_H
#define MIDICVT_HAVE_LIBINTL_H 1
#endif

/* Define to 1 if you have the <limits.h> header file. */
#ifndef MIDICVT_HAVE_LIMITS_H
#define MIDICVT_HAVE_LIMITS_H 1
#endif

/* Define to 1 if you have the <locale.h> header file. */
#ifndef MIDICVT_HAVE_LOCALE_H
#define MIDICVT_HAVE_LOCALE_H 1
#endif

/* Define to 1 if your system has a GNU libc compatible `malloc' function, and
   to 0 otherwise. */
#ifndef MIDICVT_HAVE_MALLOC
#define MIDICVT_HAVE_MALLOC 1
#endif

/* Define to 1 if you have the <math.h> header file. */
#ifndef MIDICVT_HAVE_MATH_H
#define MIDICVT_HAVE_MATH_H 1
#endif

/* Define to 1 if you have the <memory.h> header file. */
#ifndef MIDICVT_HAVE_MEMORY_H
#define MIDICVT_HAVE_MEMORY_H 1
#endif

/* Define to 1 if you have the <stdarg.h> header file. */
#ifndef MIDICVT_HAVE_STDARG_H
#define MIDICVT_HAVE_STDARG_H 1
#endif

/* Define to 1 if you have the <stddef.h> header file. */
#ifndef MIDICVT_HAVE_STDDEF_H
#define MIDICVT_HAVE_STDDEF_H 1
#endif

/* Define to 1 if you have the <stdint.h> header file. */
#ifndef MIDICVT_HAVE_STDINT_H
#define MIDICVT_HAVE_STDINT_H 1
#endif

/* Define to 1 if you have the <stdio.h> header file. */
#ifndef MIDICVT_HAVE_STDIO_H
#define MIDICVT_HAVE_STDIO_H 1
#endif

/* Define to 1 if you have the <stdlib.h> header file. */
#ifndef MIDICVT_HAVE_STDLIB_H
#define MIDICVT_HAVE_STDLIB_H 1
#endif

/* Define to 1 if you have the `strerror' function. */
#ifndef MIDICVT_HAVE_STRERROR
#define MIDICVT_HAVE_STRERROR 1
#endif

/* Define to 1 if you have the <strings.h> header file. */
#ifndef MIDICVT_HAVE_STRINGS_H
#define MIDICVT_HAVE_STRINGS_H 1
#endif

/* Define to 1 if you have the <string.h> header file. */
#ifndef MIDICVT_HAVE_STRING_H
#define MIDICVT_HAVE_STRING_H 1
#endif

/* Define to 1 if you have the <sys/select.h> header file. */
#ifndef MIDICVT_HAVE_SYS_SELECT_H
#define MIDICVT_HAVE_SYS_SELECT_H 1
#endif

/* Define to 1 if you have the <sys/socket.h> header file. */
#ifndef MIDICVT_HAVE_SYS_SOCKET_H
#define MIDICVT_HAVE_SYS_SOCKET_H 1
#endif

/* Define to 1 if you have the <sys/stat.h> header file. */
#ifndef MIDICVT_HAVE_SYS_STAT_H
#define MIDICVT_HAVE_SYS_STAT_H 1
#endif

/* Define to 1 if you have the <sys/sysctl.h> header file. */
#ifndef MIDICVT_HAVE_SYS_SYSCTL_H
#define MIDICVT_HAVE_SYS_SYSCTL_H 1
#endif

/* Define to 1 if you have the <sys/time.h> header file. */
#ifndef MIDICVT_HAVE_SYS_TIME_H
#define MIDICVT_HAVE_SYS_TIME_H 1
#endif

/* Define to 1 if you have the <sys/types.h> header file. */
#ifndef MIDICVT_HAVE_SYS_TYPES_H
#define MIDICVT_HAVE_SYS_TYPES_H 1
#endif

/* Define to 1 if you have the <time.h> header file. */
#ifndef MIDICVT_HAVE_TIME_H
#define MIDICVT_HAVE_TIME_H 1
#endif

/* Define to 1 if you have the <unistd.h> header file. */
#ifndef MIDICVT_HAVE_UNISTD_H
#define MIDICVT_HAVE_UNISTD_H 1
#endif

/* Define if not on cygwin or mingw */
#ifndef MIDICVT_LINUX
#define MIDICVT_LINUX 1
#endif

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#ifndef MIDICVT_LT_OBJDIR
#define MIDICVT_LT_OBJDIR ".libs/"
#endif

/* Define on Mingw */
/* #undef MINGW */

/* Name of package */
#ifndef MIDICVT_PACKAGE
#define MIDICVT_PACKAGE "midicvt"
#endif

/* Define to the address where bug reports for this package should be sent. */
#ifndef MIDICVT_PACKAGE_BUGREPORT
#define MIDICVT_PACKAGE_BUGREPORT "ahlstromcj@users.sourceforge.net"
#endif

/* Define to the full name of this package. */
#ifndef MIDICVT_PACKAGE_NAME
#define MIDICVT_PACKAGE_NAME "midicvt"
#endif

/* Define to the full name and version of this package. */
#ifndef MIDICVT_PACKAGE_STRING
#define MIDICVT_PACKAGE_STRING "midicvt 0.2"
#endif

/* Define to the one symbol short name of this package. */
#ifndef MIDICVT_PACKAGE_TARNAME
#define MIDICVT_PACKAGE_TARNAME "midicvt"
#endif

/* Define to the home page for this package. */
#ifndef MIDICVT_PACKAGE_URL
#define MIDICVT_PACKAGE_URL ""
#endif

/* Define to the version of this package. */
#ifndef MIDICVT_PACKAGE_VERSION
#define MIDICVT_PACKAGE_VERSION "0.2"
#endif

/* Define PROFLAGS=-pg (gprof) or -p (prof) if profile support is wanted. */
#ifndef MIDICVT_PROFLAGS
#define MIDICVT_PROFLAGS 
#endif

/* Define to the type of arg 1 for `select'. */
#ifndef MIDICVT_SELECT_TYPE_ARG1
#define MIDICVT_SELECT_TYPE_ARG1 int
#endif

/* Define to the type of args 2, 3 and 4 for `select'. */
#ifndef MIDICVT_SELECT_TYPE_ARG234
#define MIDICVT_SELECT_TYPE_ARG234 (fd_set *)
#endif

/* Define to the type of arg 5 for `select'. */
#ifndef MIDICVT_SELECT_TYPE_ARG5
#define MIDICVT_SELECT_TYPE_ARG5 (struct timeval *)
#endif

/* Define to 1 if you have the ANSI C header files. */
#ifndef MIDICVT_STDC_HEADERS
#define MIDICVT_STDC_HEADERS 1
#endif

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#ifndef MIDICVT_TIME_WITH_SYS_TIME
#define MIDICVT_TIME_WITH_SYS_TIME 1
#endif

/* Define to 1 if your <sys/time.h> declares `struct tm'. */
/* #undef TM_IN_SYS_TIME */

/* Version number of package */
#ifndef MIDICVT_VERSION
#define MIDICVT_VERSION "0.2.0"
#endif

/* Define on windows */
/* #undef WIN32 */

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define to rpl_malloc if the replacement function should be used. */
/* #undef malloc */

/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef size_t */

/* Bottom of header config */

 
/* once: _INCLUDE_MIDICVT_CONFIG_H */
#endif
