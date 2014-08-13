AUTOMAKE_OPTIONS = foreign

noinst_PROGRAMS = test_sqlite

INCLUDES = -I$(top_srcdir)/libsqlite/src

test_sqlite_LDADD = $(top_srcdir)/libsqlite/src/libsqlite3.a

LIBS = -lpthread -ldl -lgcov

test_sqlite_SOURCES = test_sqlite.c
