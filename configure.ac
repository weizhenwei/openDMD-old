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

# for libx264 needed Macro
AC_DEFINE(HAVE_GPL, 0, [have GPL(no)])
AC_DEFINE(HAVE_INTERLACED, 0, [interlaced scan(no)])

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h inttypes.h limits.h locale.h malloc.h \
                  stddef.h stdlib.h string.h strings.h sys/ioctl.h \
                  sys/param.h sys/time.h sys/timeb.h syslog.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MMAP
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([atexit bzero ftime gettimeofday memmove memset munmap \
                pow select setlocale sqrt strcasecmp strchr strcspn \
                strdup strncasecmp strpbrk strspn strstr strtol])

# set CFLAGS.
CFLAGS="-Wall -ggdb3 -std=gnu99 "

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
AC_CONFIG_FILES([Makefile
                 libjpeg/Makefile
                 libx264/Makefile
                 libx264/common/Makefile
                 libx264/encoder/Makefile
                 src/Makefile])
AC_OUTPUT
