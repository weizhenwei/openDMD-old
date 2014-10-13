AUTOMAKE_OPTIONS = foreign

SUBDIRS = client server webserver

bin_PROGRAMS = opendmd

INCLUDES = -I$(top_srcdir)/libjpeg -I$(top_srcdir)/libsqlite/src \
           -I$(top_srcdir)/libortp/include -I$(top_srcdir)/libortp \
           -I$(top_srcdir)/libx264 -I$(top_srcdir)/libx264/output \
           -I$(top_srcdir)/src/client -I$(top_srcdir)/src/server

# WARNING: there is an important problems here!
#          libclient.a and libserver.a depend on libjpeg.a, etc,
#          so libclient.a and libserver.a must replace ahead of
#          other libraries.
opendmd_LDADD = $(top_srcdir)/src/client/libclient.a \
                $(top_srcdir)/src/server/libserver.a \
                $(top_srcdir)/libjpeg/libjpeg.a \
                $(top_srcdir)/libortp/src/libortp.a \
                $(top_srcdir)/libsqlite/src/libsqlite3.a \
                $(top_srcdir)/libx264/libx264.a

# Warning: system library should be assigned here using LIBS;
# using opendmd_LDFLAGS at here doesn't work!
# using LDFLAGS in configure.ac doesn't work, either!
# -lrt for function clock_gettime()
LIBS = -lm -lpthread -lrt -ldl

#jit specific lib
# if HAVE_JIT
# opendmd_LDADD += $(top_srcdir)/src/jit/libjit.a
# INCLUDES += -I$(top_srcdir)/src/jit
# endif

# web specific lib
# if HAVE_WEBSERVER
opendmd_LDADD += $(top_srcdir)/src/webserver/libwebserver.a
INCLUDES += -I$(top_srcdir)/src/webserver
# endif

# source files in ./src dirctory;
# source files in ./src/client is in libclient.a
# source files in ./src/server is in libserver.a
opendmd_SOURCES = main.c \
          config.h config.c log.h log.c path.h path.c global_context.h \
          global_context.c parser.h parser.c v4l2_utils.h v4l2_utils.c \
          signal_handler.h signal_handler.c statistics.h statistics.c \
          sqlite_utils.h sqlite_utils.c

include_HEADERS = config.h log.h path.h global_context.h parser.h \
                  v4l2_utils.h signal_handler.h statistics.h sqlite_utils.h

