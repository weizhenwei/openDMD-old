AUTOMAKE_OPTIONS = foreign

lib_LIBRARIES = libserver.a

INCLUDES = -I$(top_srcdir)/libjpeg -I$(top_srcdir)/libsqlite/src \
           -I$(top_srcdir)/libortp/include -I$(top_srcdir)/libortp \
           -I$(top_srcdir)/libx264 -I$(top_srcdir)/libx264/output \
           -I$(top_srcdir)/src

libserver_a_SOURCES = rtp_recv.h rtp_recv.c rtp_server.h rtp_server.c

include_HEADERS = rtp_recv.h rtp_server.h

