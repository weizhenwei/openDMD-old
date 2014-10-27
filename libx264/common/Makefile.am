AUTOMAKE_OPTIONS = foreign

INCLUDES = -I$(top_srcdir)/libx264 -I$(top_srcdir)/libx264/common \
           -I$(top_srcdir)/libx264/encoder

noinst_LIBRARIES=libcommon.a
# to remove the automake complain:
#         object 'foo.$(OBJEXT)' created both with libtool and without
# libcommon_a_CFLAGS = $(AM_CFLAGS)
libcommon_a_SOURCES = bitstream.h bitstream.c \
                      cabac.h cabac.c \
                      common.h common.c \
                      cpu.h cpu.c \
                      dct.h dct.c \
                      deblock.c \
                      frame.h frame.c \
                      macroblock.h macroblock.c \
                      mc.h mc.c \
                      mvpred.c \
                      osdep.h osdep.c \
                      pixel.h pixel.c \
                      predict.h predict.c \
                      quant.h quant.c \
                      rectangle.h rectangle.c \
                      set.h set.c \
                      threadpool.h vlc.c

# noinst_LTLIBRARIES=libcommon.la
# libcommon_la_SOURCES = bitstream.h bitstream.c \
#                        cabac.h cabac.c \
#                        common.h common.c \
#                        cpu.h cpu.c \
#                        dct.h dct.c \
#                        deblock.c \
#                        frame.h frame.c \
#                        macroblock.h macroblock.c \
#                        mc.h mc.c \
#                        mvpred.c \
#                        osdep.h osdep.c \
#                        pixel.h pixel.c \
#                        predict.h predict.c \
#                        quant.h quant.c \
#                        rectangle.h rectangle.c \
#                        set.h set.c \
#                        threadpool.h vlc.c

