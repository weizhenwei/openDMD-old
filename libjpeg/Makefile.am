AUTOMAKE_OPTIONS = foreign

ACLOCAL_AMFLAGS = "-I m4"

lib_LIBRARIES = libjpeg.a
# to remove the automake complain:
#         object 'foo.$(OBJEXT)' created both with libtool and without
libjpeg_a_CFLAGS = $(AM_CFLAGS)
libjpeg_a_SOURCES = jaricom.c jcapimin.c jcapistd.c jcarith.c jccoefct.c \
                    jccolor.c jcdctmgr.c jchuff.c jcinit.c jcmainct.c \
                    jcmarker.c jcmaster.c jcomapi.c jcparam.c jcprepct.c \
                    jcsample.c jctrans.c jdapimin.c jdapistd.c jdarith.c \
                    jdatadst.c jdatasrc.c jdcoefct.c jdcolor.c jddctmgr.c \
                    jdhuff.c jdinput.c jdmainct.c jdmarker.c jdmaster.c \
                    jdmerge.c jdpostct.c jdsample.c jdtrans.c jerror.c \
                    jfdctflt.c  jfdctfst.c jfdctint.c jidctflt.c jidctfst.c \
                    jidctint.c jquant1.c jquant2.c jutils.c jmemmgr.c \
                    jmemnobs.c jerror.h jmorecfg.h jpeglib.h cderror.h \
                    cdjpeg.h jdct.h jinclude.h jmemsys.h jpegint.h \
                    jversion.h transupp.h jconfig.h

LIB_AC=1
LIB_REV=0
LIB_ANC=1
lib_LTLIBRARIES = libjpeg.la
libjpeg_la_CFLAGS = $(AM_CFLAGS)
libjpeg_la_LDFLAGS = -version-info ${LIB_AC}:${LIB_REV}:${LIB_ANC}
libjpeg_la_SOURCES = jaricom.c jcapimin.c jcapistd.c jcarith.c jccoefct.c \
                     jccolor.c jcdctmgr.c jchuff.c jcinit.c jcmainct.c \
                     jcmarker.c jcmaster.c jcomapi.c jcparam.c jcprepct.c \
                     jcsample.c jctrans.c jdapimin.c jdapistd.c jdarith.c \
                     jdatadst.c jdatasrc.c jdcoefct.c jdcolor.c jddctmgr.c \
                     jdhuff.c jdinput.c jdmainct.c jdmarker.c jdmaster.c \
                     jdmerge.c jdpostct.c jdsample.c jdtrans.c jerror.c \
                     jfdctflt.c  jfdctfst.c jfdctint.c jidctflt.c jidctfst.c \
                     jidctint.c jquant1.c jquant2.c jutils.c jmemmgr.c \
                     jmemnobs.c jerror.h jmorecfg.h jpeglib.h cderror.h \
                     cdjpeg.h jdct.h jinclude.h jmemsys.h jpegint.h \
                     jversion.h transupp.h jconfig.h


include_HEADERS = jerror.h jmorecfg.h jpeglib.h

## Headers which are installed to support the library
#INSTINCLUDES = jerror.h jmorecfg.h jpeglib.h
#
## Headers which are not installed
#OTHERINCLUDES = cderror.h cdjpeg.h jdct.h jinclude.h jmemsys.h jpegint.h \
#        jversion.h transupp.h
