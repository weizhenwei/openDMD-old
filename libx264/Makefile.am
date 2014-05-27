AUTOMAKE_OPTIONS = foreign

SUBDIRS = common encoder

lib_LIBRARIES=libx264.a

INCLUDES = -I$(top_srcdir)/libx264 -I$(top_srcdir)/libx264/common \
		   -I$(top_srcdir)/libx264/encoder

#this method can work, but ugly.
libx264_a_SOURCES = x264.h x264_config.h
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
				   encoder/lookahead.o

