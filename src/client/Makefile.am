AUTOMAKE_OPTIONS = foreign

noinst_LIBRARIES = libclient.a

INCLUDES = -I$(top_srcdir)/libjpeg -I$(top_srcdir)/libsqlite/src \
           -I$(top_srcdir)/libortp/include -I$(top_srcdir)/libortp \
           -I$(top_srcdir)/libx264 -I$(top_srcdir)/libx264/output \
           -I$(top_srcdir)/src
# libclient_a_LIBADD = $(top_srcdir)/libjpeg/libjpeg.a \
#                      $(top_srcdir)/libortp/src/libortp.a \
#                      $(top_srcdir)/libsqlite/src/libsqlite3.a \
#                      $(top_srcdir)/libx264/libx264.a

libclient_a_SOURCES = flv_muxer.c image_capture.c image_convert.c \
                      libx264.c picture_thread.h rtp_send.h video.h \
                      video_thread.h flv_muxer.h image_capture.h \
                      image_convert.h  libx264.h  picture_thread.c \
                      rtp_send.c video.c video_thread.c

include_HEADERS = flv_muxer.h image_capture.h image_convert.h libx264.h \
                  picture_thread.h rtp_send.h video.h video_thread.h

