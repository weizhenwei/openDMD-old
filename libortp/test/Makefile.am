AUTOMAKE_OPTIONS = foreign

noinst_PROGRAMS = sendtest recvtest mrtprecv

INCLUDES = -I$(top_srcdir)/libortp/include -I$(top_srcdir)/libortp \
		   -I$(top_srcdir)/libortp/src

if ENABLE_DYNAMIC
sendtest_LDADD = $(top_srcdir)/libortp/src/libortp.la
recvtest_LDADD = $(top_srcdir)/libortp/src/libortp.la
mrtprecv_LDADD = $(top_srcdir)/libortp/src/libortp.la
else
sendtest_LDADD = $(top_srcdir)/libortp/src/libortp.a
recvtest_LDADD = $(top_srcdir)/libortp/src/libortp.a
mrtprecv_LDADD = $(top_srcdir)/libortp/src/libortp.a
endif

# Warning: system library should be assigned here using LIBS;
# using openDMD_LDFLAGS at here doesn't work!
LIBS = -lrt

sendtest_SOURCES = rtp_send.h rtp_send.c send_test.c
recvtest_SOURCES = rtp_recv.h rtp_recv.c recv_test.c
mrtprecv_SOURCES = mrtprecv.c

