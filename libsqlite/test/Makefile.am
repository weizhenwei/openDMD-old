AUTOMAKE_OPTIONS = foreign

noinst_PROGRAMS = test_basic test_select

INCLUDES = -I$(top_srcdir)/libsqlite/src
LIBS = -lpthread -ldl

if ENABLE_DYNAMIC
test_basic_LDADD = $(top_srcdir)/libsqlite/src/libsqlite3.la
test_basic_SOURCES = test_basic.c

test_select_LDADD = $(top_srcdir)/libsqlite/src/libsqlite3.la
test_select_SOURCES = test_select.c

else

test_basic_LDADD = $(top_srcdir)/libsqlite/src/libsqlite3.a
test_basic_SOURCES = test_basic.c

test_select_LDADD = $(top_srcdir)/libsqlite/src/libsqlite3.a
test_select_SOURCES = test_select.c


endif

