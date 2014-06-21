AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = opendmd

INCLUDES = -I$(top_srcdir)/libccl -I$(top_srcdir)/libjpeg \
		   -I$(top_srcdir)/libortp/include -I$(top_srcdir)/libortp \
		   -I$(top_srcdir)/libx264 -I$(top_srcdir)/src/client \
		   -I$(top_srcdir)/src/server

opendmd_LDADD = $(top_srcdir)/libccl/libccl.a \
				$(top_srcdir)/libjpeg/libjpeg.a \
				$(top_srcdir)/libx264/libx264.a \
				$(top_srcdir)/libortp/src/libortp.a

# Warning: system library should be assigned here using LIBS;
# using opendmd_LDFLAGS at here doesn't work!
# using LDFLAGS in configure.ac doesn't work, either!
# -lrt for function clock_gettime()
LIBS=-lm -lpthread -lrt

opendmd_SOURCES = main.c \
                  config.h config.c log.h log.c path.h path.c \
                  global_context.h global_context.c \
                  client/video.h client/video.c \
				  v4l2_utils.h v4l2_utils.c \
                  signal_handler.h signal_handler.c \
                  client/image_convert.h client/image_convert.c \
                  client/image_capture.h client/image_capture.c \
				  client/picture_thread.h client/picture_thread.c \
                  client/libx264.h client/libx264.c \
				  client/video_thread.h client/video_thread.c \
				  client/rtp_send.h client/rtp_send.c

