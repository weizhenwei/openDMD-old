AUTOMAKE_OPTIONS = foreign

noinst_PROGRAMS = sendtest recvtest mrtprecv

INCLUDES = -I$(top_srcdir)/libortp/include -I$(top_srcdir)/libortp \
		   -I$(top_srcdir)/libortp/src

sendtest_LDADD = $(top_srcdir)/libortp/src/libortp.a
recvtest_LDADD = $(top_srcdir)/libortp/src/libortp.a
mrtprecv_LDADD = $(top_srcdir)/libortp/src/libortp.a


# Warning: system library should be assigned here using LIBS;
# using opendmd_LDFLAGS at here doesn't work!
LIBS = -lrt

sendtest_SOURCES = rtp_send.h rtp_send.c send_test.c
recvtest_SOURCES = rtp_recv.h rtp_recv.c recv_test.c
mrtprecv_SOURCES = mrtprecv.c

