AUTOMAKE_OPTIONS = foreign

SUBDIRS = common encoder output

INCLUDES = -I$(top_srcdir)/libx264 -I$(top_srcdir)/libx264/common \
           -I$(top_srcdir)/libx264/encoder -I$(top_srcdir)/libx264/output

lib_LIBRARIES=libx264.a
#this method can work, but ugly.
libx264_a_SOURCES = x264.h x264_config.h x264cli.h
# to remove the automake complain:
#         object 'foo.$(OBJEXT)' created both with libtool and without
# libx264_a_CFLAGS = $(AM_CFLAGS)
libx264_a_LIBADD = common/mc.o \
                   common/predict.o \
                   common/pixel.o \
                   common/macroblock.o \
                   common/frame.o \
                   common/dct.o \
                   common/cpu.o \
                   common/cabac.o \
                   common/common.o \
                   common/osdep.o \
                   common/rectangle.o \
                   common/set.o \
                   common/quant.o \
                   common/deblock.o \
                   common/vlc.o \
                   common/mvpred.o \
                   common/bitstream.o \
                   encoder/analyse.o \
                   encoder/me.o \
                   encoder/ratecontrol.o \
                   encoder/set.o \
                   encoder/macroblock.o \
                   encoder/cabac.o \
                   encoder/cavlc.o \
                   encoder/encoder.o \
                   encoder/lookahead.o \
                   output/flv_bytestream.o \
                   output/flv.o \
                   output/raw.o

# lib_LTLIBRARIES=libx264.la
# libx264_la_CFLAGS = $(AM_CFLAGS)
# libx264_la_LDFLAGS = -avoid-version -no-undefined
# #this method can work, but ugly.
# libx264_la_SOURCES = x264.h x264_config.h x264cli.h
# libx264_la_LIBADD = common/mc.lo \
#                     common/predict.lo \
#                     common/pixel.lo \
#                     common/macroblock.lo \
#                     common/frame.lo \
#                     common/dct.lo \
#                     common/cpu.lo \
#                     common/cabac.lo \
#                     common/common.lo \
#                     common/osdep.lo \
#                     common/rectangle.lo \
#                     common/set.lo \
#                     common/quant.lo \
#                     common/deblock.lo \
#                     common/vlc.lo \
#                     common/mvpred.lo \
#                     common/bitstream.lo \
#                     encoder/analyse.lo \
#                     encoder/me.lo \
#                     encoder/ratecontrol.lo \
#                     encoder/set.lo \
#                     encoder/macroblock.lo \
#                     encoder/cabac.lo \
#                     encoder/cavlc.lo \
#                     encoder/encoder.lo \
#                     encoder/lookahead.lo \
#                     output/flv_bytestream.lo \
#                     output/flv.lo \
#                     output/raw.lo

include_HEADERS = x264.h x264cli.h  x264_config.h

