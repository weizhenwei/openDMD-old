#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([opendmd], [0.0.1], [weizhenwei1988@gmail.com])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])

# get system canonical system message,
# such as build, host, target;
#         build_alias, host_alias, arget_alias
#         build_cpu,   build_vender, build_os
#         host_cpu,    host_vender,  host_os
#         target_cpu,   target_vender, target_os
AC_CANONICAL_SYSTEM

AM_INIT_AUTOMAKE(opendmd, 0.0.1)

# Checks for programs.
AC_PROG_CC         # for c compiler
# AC_PROG_LIBTOOL    # for libtool
# AC_PROG_LEX   # for flex
# AC_PROG_YACC  # for bison

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
AC_C_RESTRICT
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
AC_ARG_ENABLE( [debug], AS_HELP_STRING([--enable-debug], [enable debug mode(default=no)]),
          [],
          [enable_debug=no]
          )
AS_IF([test "x$enable_debug" = "xyes"], [CFLAGS+="-DDEBUG "],
      [test "x$enable_debug" = "xno"], [CFLAGS+=" "],
      [echo "enable-debug with wrong value"]
      )


# add --enable-gcov option.
AC_ARG_ENABLE( [gcov], 
          AS_HELP_STRING([--enable-gcov], [enable gcov code-coverage(default=no)]),
          [],
          [enable_gcov=no]
          )
# add code-coverage info to gcc options;
AS_IF([test "x$enable_gcov" = "xyes"], [CFLAGS+="-fprofile-arcs -ftest-coverage "
       LDFLAGS+="-lgcov "],
      [test "x$enable_gcov" = "xno"], [CFLAGS+=" " LDFLAGS+=" "],
      [echo "enable-gcov with wrong value"]
      )
# AM_CONDITIONAL([HAVE_GCOV], [test "x$enable_gcov" = xyes])

# add --enable-jit option.
# AC_ARG_ENABLE( [jit], 
#           AS_HELP_STRING([--enable-jit], [enable jit(default=no)]),
#           [],
#           [enable_jit=no]
#           )
# AS_IF([test "x$enable_jit" = "xyes"], [CFLAGS+="-DJIT "],
#       [test "x$enable_jit" = "xno"], [CFLAGS+=" "],
#       [echo "enable-jit with wrong value"]
#       )
# AM_CONDITIONAL([HAVE_JIT], [test "x$enable_jit" = xyes])

# choose specific host architecture
AM_CONDITIONAL([HAVE_X86_64], [test "x$host_cpu" = xx86_64])
AM_CONDITIONAL([HAVE_ARM], [test "x$host_cpu" = xarm])



# add --enable-web option.
AC_ARG_ENABLE( [web], 
          AS_HELP_STRING([--enable-web], [enable web admin(default=no)]),
          [],
          [enable_web=no]
          )
AS_IF([test "x$enable_web" = "xyes"], [CFLAGS+="-DWEB "],
      [test "x$enable_web" = "xno"], [CFLAGS+=" "],
      [echo "enable-web with wrong value"]
      )
AM_CONDITIONAL([HAVE_WEBSERVER], [test "x$enable_web" = xyes])

#Attention: this is import, list the Makefile need to generate
AC_CONFIG_FILES([Makefile
                 libjpeg/Makefile
                 libortp/Makefile
                 libortp/include/Makefile
                 libortp/include/ortp/Makefile
                 libortp/src/Makefile
                 libortp/test/Makefile
                 libsqlite/Makefile
                 libsqlite/src/Makefile
                 libsqlite/test/Makefile
                 libx264/Makefile
                 libx264/common/Makefile
                 libx264/encoder/Makefile
                 libx264/output/Makefile
                 src/Makefile
                 src/client/Makefile
                 src/server/Makefile
                 src/webserver/Makefile
                 ])
AC_OUTPUT
