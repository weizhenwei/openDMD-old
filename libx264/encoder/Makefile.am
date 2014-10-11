AUTOMAKE_OPTIONS = foreign

noinst_LIBRARIES=libencoder.a

INCLUDES = -I$(top_srcdir)/libx264 -I$(top_srcdir)/libx264/common \
           -I$(top_srcdir)/libx264/encoder

libencoder_a_SOURCES = analyse.h analyse.c \
                       cabac.c cavlc.c \
                       encoder.c lookahead.c \
                       macroblock.h macroblock.c \
                       me.h me.c \
                       ratecontrol.h ratecontrol.c \
                       set.h set.c

EXTRA_DIST = rdo.c slicetype.c


