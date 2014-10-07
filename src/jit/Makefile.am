AUTOMAKE_OPTIONS=foreign

SUBDIRS = test

lib_LIBRARIES = libjit.a

INCLUDES = -I$(top_srcdir)/libjpeg -I$(top_srcdir)/libsqlite/src \
           -I$(top_srcdir)/libortp/include -I$(top_srcdir)/libortp \
           -I$(top_srcdir)/libx264 -I$(top_srcdir)/libx264/output

# libjit_LIBADD = $(top_srcdir)/libjpeg/libjpeg.a \
#                 $(top_srcdir)/libortp/src/libortp.a \
#                 $(top_srcdir)/libsqlite/src/libsqlite3.a \
#                 $(top_srcdir)/libx264/libx264.a

JITFILES := jit.h jit.c
# architecture specific files
if HAVE_X86_64
    JITFILES += x86_64/x86_64_assembler.h x86_64/x86_64_assembler.c
endif
if HAVE_ARM
    JITFILES += arm/arm_assembler.h arm/arm_assembler.c
endif

libjit_a_SOURCES = $(JITFILES)

include_HEADERS = jit.h

