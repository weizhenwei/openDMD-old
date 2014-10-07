AUTOMAKE_OPTIONS=foreign

lib_LIBRARIES = libwebserver.a

INCLUDES = -I$(top_srcdir)/libjpeg -I$(top_srcdir)/libsqlite/src \
           -I$(top_srcdir)/libortp/include -I$(top_srcdir)/libortp \
           -I$(top_srcdir)/libx264 -I$(top_srcdir)/libx264/output

libwebserver_a_SOURCES = socket_utils.h socket_utils.c \
                         http_response-inl.h http_utils.h http_utils.c \
                         webserver_process.h webserver_process.c

include_HEADERS = socket_utils.h http_utils.h webserver_process.h
