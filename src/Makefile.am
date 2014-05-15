AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = opendmd

INCLUDES = -I$(top_srcdir)/libjpeg

opendmd_LDADD = $(top_srcdir)/libjpeg/libjpeg.a

opendmd_SOURCES = main.c \
	       dmd_log.h dmd_log.c \
	       dmd_video.h dmd_video.c \
	       dmd_image.h dmd_image.c \
	       v4l2_utils.h v4l2_utils.c
