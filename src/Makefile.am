AUTOMAKE_OPTIONS = foreign

SUBDIRS = client server webserver

bin_PROGRAMS = openDMD

INCLUDES = -I$(top_srcdir)/libjpeg -I$(top_srcdir)/libsqlite/src \
           -I$(top_srcdir)/libortp/include -I$(top_srcdir)/libortp \
           -I$(top_srcdir)/libx264 -I$(top_srcdir)/libx264/output \
           -I$(top_srcdir)/src/client -I$(top_srcdir)/src/server \
           -I$(top_srcdir)/src/webserver

if ENABLE_DYNAMIC
# WARNING: there is an important problems here!
#          obj files at src/client and src/server depend on libjpeg.a, etc,
#          so obj files at these two dir must place ahead of other libraries.
openDMD_LDADD = $(top_srcdir)/src/client/flv_muxer.o \
                $(top_srcdir)/src/client/image_capture.o \
                $(top_srcdir)/src/client/image_convert.o \
                $(top_srcdir)/src/client/libx264.o \
                $(top_srcdir)/src/client/picture_thread.o \
                $(top_srcdir)/src/client/rtp_send.o \
                $(top_srcdir)/src/client/video.o \
                $(top_srcdir)/src/client/video_thread.o \
                $(top_srcdir)/src/server/rtp_server.o \
                $(top_srcdir)/src/server/rtp_recv.o \
                $(top_srcdir)/src/webserver/http_utils.o \
                $(top_srcdir)/src/webserver/socket_utils.o \
                $(top_srcdir)/src/webserver/webserver_process.o \
                $(top_srcdir)/libjpeg/libjpeg.la \
                $(top_srcdir)/libortp/src/libortp.la \
                $(top_srcdir)/libsqlite/src/libsqlite3.la \
                $(top_srcdir)/libx264/libx264.la 

else

openDMD_LDADD = $(top_srcdir)/src/client/flv_muxer.o \
                $(top_srcdir)/src/client/image_capture.o \
                $(top_srcdir)/src/client/image_convert.o \
                $(top_srcdir)/src/client/libx264.o \
                $(top_srcdir)/src/client/picture_thread.o \
                $(top_srcdir)/src/client/rtp_send.o \
                $(top_srcdir)/src/client/video.o \
                $(top_srcdir)/src/client/video_thread.o \
                $(top_srcdir)/src/server/rtp_server.o \
                $(top_srcdir)/src/server/rtp_recv.o \
                $(top_srcdir)/src/webserver/http_utils.o \
                $(top_srcdir)/src/webserver/socket_utils.o \
                $(top_srcdir)/src/webserver/webserver_process.o \
                $(top_srcdir)/libjpeg/libjpeg.a \
                $(top_srcdir)/libortp/src/libortp.a \
                $(top_srcdir)/libsqlite/src/libsqlite3.a \
                $(top_srcdir)/libx264/libx264.a 

endif

# Warning: system library should be assigned here using LIBS;
# using openDMD_LDFLAGS at here doesn't work!
# using LDFLAGS in configure.ac doesn't work, either!
# -lrt for function clock_gettime()
LIBS = -lm -lpthread -lrt -ldl

# source files in ./src dirctory;
# source files in ./src/client is in openDMD_LDADD src/client/*.o
# source files in ./src/server is in openDMD_LDADD src/server/*.o 
# source files in ./src/webserver is in openDMD_LDADD src/webserver/*.o 
openDMD_SOURCES = main.c \
          config.h config.c log.h log.c path.h path.c global_context.h \
          global_context.c parser.h parser.c v4l2_utils.h v4l2_utils.c \
          signal_handler.h signal_handler.c statistics.h statistics.c \
          sqlite_utils.h sqlite_utils.c

include_HEADERS = config.h log.h path.h global_context.h parser.h \
                  v4l2_utils.h signal_handler.h statistics.h sqlite_utils.h

