AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = opendmd

INCLUDES = -I$(top_srcdir)/libccl -I$(top_srcdir)/libjpeg -I$(top_srcdir)/libx264 

opendmd_LDADD = $(top_srcdir)/libccl/libccl.a \
				$(top_srcdir)/libjpeg/libjpeg.a \
				$(top_srcdir)/libx264/libx264.a

# Warning: system library should be assigned here using LIBS;
# using opendmd_LDFLAGS at here doesn't work!
# using LDFLAGS in configure.ac doesn't work, either!
LIBS=-lm -lpthread

opendmd_SOURCES = main.c \
                  dmd_config.h dmd_config.c \
                  dmd_global_context.h dmd_global_context.c \
                  dmd_log.h dmd_log.c dmd_path.h dmd_path.c \
                  dmd_video.h dmd_video.c \
                  dmd_signal.h dmd_signal.c \
                  dmd_v4l2_utils.h dmd_v4l2_utils.c \
                  dmd_image_convert.h dmd_image_convert.c \
                  dmd_image_capture.h dmd_image_capture.c \
                  dmd_x264.h dmd_x264.c \
				  dmd_picture_thread.h dmd_picture_thread.c \
				  dmd_video_thread.h dmd_video_thread.c

