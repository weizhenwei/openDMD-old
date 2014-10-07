AUTOMAKE_OPTIONS = foreign

noinst_PROGRAMS = test_jit

INCLUDES = -I$(top_srcdir)/libjpeg -I$(top_srcdir)/libsqlite/src \
           -I$(top_srcdir)/libortp/include -I$(top_srcdir)/libortp \
           -I$(top_srcdir)/libx264 -I$(top_srcdir)/libx264/output \
           -I$(top_srcdir)/src/client -I$(top_srcdir)/src/server

# test_jit_LDADD = $(top_srcdir)/libjpeg/libjpeg.a \
#                  $(top_srcdir)/libortp/src/libortp.a \
#                  $(top_srcdir)/libsqlite/src/libsqlite3.a \
#                  $(top_srcdir)/libx264/libx264.a

# jit specific files
JITFILES := ../jit.h ../jit.c
if HAVE_X86_64
    JITFILES += ../x86_64/x86_64_assembler.h ../x86_64/x86_64_assembler.c
endif
if HAVE_ARM
    JITFILES += ../arm/arm_assembler.h ../arm/arm_assembler.c
endif

test_jit_SOURCES = test_jit.c $(JITFILES)

