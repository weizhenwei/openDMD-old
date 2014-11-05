AUTOMAKE_OPTIONS = foreign

lib_LIBRARIES = libsqlite3.a
# to remove the automake complain:
#         object 'foo.$(OBJEXT)' created both with libtool and without
libsqlite3_a_CFLAGS = $(AM_CFLAGS)
libsqlite3_a_SOURCES = sqlite3.c sqlite3.h


LIB_AC=1
LIB_REV=0
LIB_ANC=1
lib_LTLIBRARIES = libsqlite3.la
# to remove the automake complain:
#         object 'foo.$(OBJEXT)' created both with libtool and without
libsqlite3_la_CFLAGS = $(AM_CFLAGS)
libsqlite3_la_LDFLAGS = -version-info ${LIB_AC}:${LIB_REV}:${LIB_ANC}
libsqlite3_la_SOURCES = sqlite3.c sqlite3.h

include_HEADERS = sqlite3.h


noinst_PROGRAMS = sqlite3

# TODO: gcc options when linking with dlopen library: -rdynamic -ldl
LIBS = -lpthread -ldl

sqlite3_SOURCES = shell.c  sqlite3.c  sqlite3ext.h  sqlite3.h

