AUTOMAKE_OPTIONS = foreign

ACLOCAL_AMFLAGS = "-I m4"

INCLUDES = -I$(top_srcdir)/libortp/include -I$(top_srcdir)/libortp

lib_LIBRARIES = libortp.a
# libortp_a_LIBADD= -lm -lpthread -lrt
# to remove the automake complain:
#         object 'foo.$(OBJEXT)' created both with libtool and without
libortp_a_CFLAGS = $(AM_CFLAGS)
libortp_a_SOURCES = str_utils.c \
                    port.c \
                    logging.c \
                    rtpparse.c  \
                    rtpsession.c \
                    rtpsession_inet.c \
                    rtpsession_priv.h \
                    jitterctl.c jitterctl.h \
                    rtpsignaltable.c  \
                    rtptimer.c rtptimer.h \
                    posixtimer.c \
                    ortp.c \
                    scheduler.c scheduler.h \
                    avprofile.c  \
                    sessionset.c  \
                    telephonyevents.c  \
                    payloadtype.c \
                    rtpprofile.c \
                    rtcp.c  \
                    utils.c utils.h \
                    rtcpparse.c \
                    event.c \
                    stun.c stun_udp.c \
                    ortp_srtp.c \
                    b64.c \
                    zrtp.c \
                    netsim.c

lib_LTLIBRARIES = libortp.la
# libortp_la_LIBADD= -lm -lpthread -lrt
libortp_la_SOURCES = str_utils.c \
                     port.c \
                     logging.c \
                     rtpparse.c  \
                     rtpsession.c \
                     rtpsession_inet.c \
                     rtpsession_priv.h \
                     jitterctl.c jitterctl.h \
                     rtpsignaltable.c  \
                     rtptimer.c rtptimer.h \
                     posixtimer.c \
                     ortp.c \
                     scheduler.c scheduler.h \
                     avprofile.c  \
                     sessionset.c  \
                     telephonyevents.c  \
                     payloadtype.c \
                     rtpprofile.c \
                     rtcp.c  \
                     utils.c utils.h \
                     rtcpparse.c \
                     event.c \
                     stun.c stun_udp.c \
                     ortp_srtp.c \
                     b64.c \
                     zrtp.c \
                     netsim.c

