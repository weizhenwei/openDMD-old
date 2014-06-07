AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = opendmd

INCLUDES = -I$(top_srcdir)/libccl -I$(top_srcdir)/libjpeg \
		   -I$(top_srcdir)/libx264 

opendmd_LDADD = $(top_srcdir)/libccl/libccl.a \
				$(top_srcdir)/libjpeg/libjpeg.a \
				$(top_srcdir)/libx264/libx264.a

# Warning: system library should be assigned here using LIBS;
# using opendmd_LDFLAGS at here doesn't work!
# using LDFLAGS in configure.ac doesn't work, either!
LIBS=-lm -lpthread

opendmd_SOURCES = main.c \
                  config.h config.c log.h log.c path.h path.c \
                  global_context.h global_context.c \
                  video.h video.c v4l2_utils.h v4l2_utils.c \
                  signal_handler.h signal_handler.c \
                  image_convert.h image_convert.c \
                  image_capture.h image_capture.c \
				  picture_thread.h picture_thread.c \
                  libx264.h libx264.c video_thread.h video_thread.c

