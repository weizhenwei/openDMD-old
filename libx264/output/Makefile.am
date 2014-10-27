AUTOMAKE_OPTIONS = foreign

INCLUDES = -I$(top_srcdir)/libx264 -I$(top_srcdir)/libx264/common \
           -I$(top_srcdir)/libx264/encoder

noinst_LIBRARIES=liboutput.a
# to remove the automake complain:
#         object 'foo.$(OBJEXT)' created both with libtool and without
# liboutput_a_CFLAGS = $(AM_CFLAGS)
liboutput_a_SOURCES = flv_bytestream.c  flv_bytestream.h \
                      flv.c output.h  raw.c

# noinst_LTLIBRARIES=liboutput.la
# liboutput_la_SOURCES = flv_bytestream.c  flv_bytestream.h \
#                        flv.c output.h  raw.c

