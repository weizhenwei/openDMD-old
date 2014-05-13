#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([opendmd], [0.0.1], [weizhenwei1988@gmail.com])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE(opendmd, 0.0.1)

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h locale.h malloc.h stdlib.h string.h strings.h sys/ioctl.h sys/time.h syslog.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([bzero setlocale])

#Attention: this is import, list the Makefile need to generate
#AC_OUTPUT(Makefile src/Makefile)
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
