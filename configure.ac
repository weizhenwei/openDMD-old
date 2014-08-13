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
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [log10])
# FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread], [pthread_create])
# FIXME: Replace `main' with a function in `-lrt':
AC_CHECK_LIB([rt], [clock_gettime])

# Checks for header files.
AC_HEADER_RESOLV
AC_CHECK_HEADERS([arpa/inet.h fcntl.h inttypes.h limits.h locale.h netdb.h \
                  malloc.h etdb.h netinet/in.h stddef.h stdint.h stdlib.h \
                  string.h strings.h sys/file.h sys/ioctl.h sys/mount.h \
                  sys/param.h sys/socket.h sys/statvfs.h sys/time.h \
                  sys/timeb.h syslog.h unistd.h utime.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_UID_T
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_CHECK_MEMBERS([struct stat.st_blksize])
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_ALLOCA
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_FUNC_STRERROR_R
AC_FUNC_STRTOD
AC_CHECK_FUNCS([atexit bzero clock_gettime fdatasync ftime gethostbyname \
                gethrtime gettimeofday inet_ntoa localtime_r memmove memset \
                mkdir munmap pow select setlocale socket sqrt strcasecmp \
                strchr strcspn strdup strerror strncasecmp strpbrk strspn \
                strstr strtol utime])

# set CFLAGS.
CFLAGS="-Wall -ggdb3 -O0 -std=gnu99 "
# set LDFLAGS.
LDFLAGS=


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

# add --enable-gcov option.
AC_ARG_ENABLE( [gcov], 
	      AS_HELP_STRING([--enable-gcov], [enable gcov code-coverage(default=no)]),
	      [enable_debug=yes],
	      [enable_debug=no]
	      )
# add code-coverage info to gcc options;
AS_IF([test "x$enable_gcov" = "xyes"], [CFLAGS+="-fprofile-arcs -ftest-coverage"
       LDFLAGS+="-lgcov"],
      [test "x$enable_gcov" = "xno"], [CFLAGS+=" " LDFLAGS+=" "],
      [echo "enable-gcov with wrong value"]
      )

#Attention: this is import, list the Makefile need to generate
AC_CONFIG_FILES([Makefile
                 libjpeg/Makefile
                 libortp/Makefile
                 libortp/include/Makefile
                 libortp/include/ortp/Makefile
                 libortp/src/Makefile
                 libortp/test/Makefile
                 libsqlite/Makefile
                 libsqlite/test/Makefile
                 libx264/Makefile
                 libx264/common/Makefile
                 libx264/encoder/Makefile
                 libx264/output/Makefile
                 src/Makefile])
AC_OUTPUT
