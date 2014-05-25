#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([opendmd], [0.0.1], [weizhenwei1988@gmail.com])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE(opendmd, 0.0.1)

# Checks for programs.
AC_PROG_CC

# for static library generation
AC_PROG_RANLIB

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h locale.h malloc.h stddef.h stdlib.h string.h \
		  strings.h sys/ioctl.h sys/time.h syslog.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MMAP
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_CHECK_FUNCS([bzero munmap select setlocale])

# set CFLAGS.
CFLAGS="-Wall "
# add --enable-debug option.
AC_ARG_ENABLE( [debug], 
	      AS_HELP_STRING([--enable-debug], [enable debug mode(default=no)]),
	      [enable_debug=yes],
	      [enable_debug=no]
	      )
AS_IF([test "x$enable_debug" = "xyes"], [CFLAGS+="-DDEBUG "],
      [test "x$enable_debug" = "xno"], [CFLAGS+=" "],
      [echo "enable-debug with wrong value"]
      )


#Attention: this is import, list the Makefile need to generate
#AC_OUTPUT(Makefile src/Makefile)
AC_CONFIG_FILES([Makefile \
		 libjpeg/Makefile \
		 src/Makefile])
AC_OUTPUT
