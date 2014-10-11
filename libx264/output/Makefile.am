AUTOMAKE_OPTIONS = foreign

noinst_LIBRARIES=liboutput.a

INCLUDES = -I$(top_srcdir)/libx264 -I$(top_srcdir)/libx264/common \
           -I$(top_srcdir)/libx264/encoder

liboutput_a_SOURCES = flv_bytestream.c  flv_bytestream.h \
                      flv.c matroska.c  matroska_ebml.c  matroska_ebml.h \
                      mp4.c  mp4_lsmash.c  output.h  raw.c

