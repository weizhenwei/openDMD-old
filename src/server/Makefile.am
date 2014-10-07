AUTOMAKE_OPTIONS = foreign

noinst_LIBRARIES = libserver.a

INCLUDES = -I$(top_srcdir)/libjpeg -I$(top_srcdir)/libsqlite/src \
           -I$(top_srcdir)/libortp/include -I$(top_srcdir)/libortp \
           -I$(top_srcdir)/libx264 -I$(top_srcdir)/libx264/output \
           -I$(top_srcdir)/src
# libserver_a_LIBADD = $(top_srcdir)/libjpeg/libjpeg.a \
#                      $(top_srcdir)/libortp/src/libortp.a \
#                      $(top_srcdir)/libsqlite/src/libsqlite3.a \
#                      $(top_srcdir)/libx264/libx264.a

libserver_a_SOURCES = rtp_recv.h rtp_recv.c rtp_server.h rtp_server.c

include_HEADERS = rtp_recv.h rtp_server.h
