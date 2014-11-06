AUTOMAKE_OPTIONS = foreign

INCLUDES = -I$(top_srcdir)/libx264 -I$(top_srcdir)/libx264/common \
           -I$(top_srcdir)/libx264/encoder

if ENABLE_DYNAMIC
noinst_LTLIBRARIES=libencoder.la
libencoder_la_SOURCES = analyse.h analyse.c \
                        cabac.c cavlc.c \
                        encoder.c lookahead.c \
                        macroblock.h macroblock.c \
                        me.h me.c \
                        ratecontrol.h ratecontrol.c \
                        set.h set.c

else

noinst_LIBRARIES=libencoder.a
# to remove the automake complain:
#         object 'foo.$(OBJEXT)' created both with libtool and without
libencoder_a_CFLAGS = $(AM_CFLAGS)
libencoder_a_SOURCES = analyse.h analyse.c \
                       cabac.c cavlc.c \
                       encoder.c lookahead.c \
                       macroblock.h macroblock.c \
                       me.h me.c \
                       ratecontrol.h ratecontrol.c \
                       set.h set.c
endif

EXTRA_DIST = rdo.c slicetype.c


