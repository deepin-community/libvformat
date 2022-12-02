# Process this file with autoconf to produce a configure script.
AC_INIT(libvformat, 1.13, Nick <Tilda@indigo8.freeserve.co.uk>)
AC_CONFIG_SRCDIR([vf_iface.h])
#AC_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE(libvformat, 1.13)
AM_CONFIG_HEADER(config.h)

# Checks for programs.
# AC_PROG_CC

# Checks for libraries.
AC_PROG_RANLIB
AC_PROG_LIBTOOL

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_MKTIME
AC_FUNC_STAT
AC_CHECK_FUNCS([atexit memset strstr])

AC_OUTPUT([Makefile src/Makefile vformat/Makefile test/Makefile doc/Makefile])
AC_CONFIG_FILES([])

