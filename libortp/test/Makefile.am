AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = sendtest recvtest

INCLUDES = -I$(top_srcdir)/libortp/include -I$(top_srcdir)/libortp

sendtest_LDADD = $(top_srcdir)/libortp/src/libortp.a
recvtest_LDADD = $(top_srcdir)/libortp/src/libortp.a

# Warning: system library should be assigned here using LIBS;
# using opendmd_LDFLAGS at here doesn't work!
# LIBS=-lm -lpthread

sendtest_SOURCES = rtp_send.h rtp_send.c send_test.c
recvtest_SOURCES = rtp_recv.h rtp_recv.c recv_test.c

