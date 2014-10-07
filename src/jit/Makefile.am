AUTOMAKE_OPTIONS=foreign

SUBDIRS = test

lib_LIBRARIES = libjit.a

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

