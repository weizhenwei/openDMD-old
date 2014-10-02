AUTOMAKE_OPTIONS = foreign

noinst_PROGRAMS = test_jit

INCLUDES = -I$(top_srcdir)

# jit specific files
JITFILES := ../jit.h ../jit.c
if HAVE_X86_64
    JITFILES += ../x86_64/x86_64_assembler.h ../x86_64/x86_64_assembler.c
endif
if HAVE_ARM
    JITFILES += ../arm/arm_assembler.h ../arm/arm_assembler.c
endif

test_jit_SOURCES = test_jit.c $(JITFILES)

