AUTOMAKE_OPTIONS = foreign

lib_LIBRARIES = libsqlite3.a

libsqlite3_a_SOURCES = sqlite3.c sqlite3.h

include_HEADERS = sqlite3.h


noinst_PROGRAMS = sqlite3

# TODO: gcc options when linking with dlopen library: -rdynamic -ldl
LIBS = -lpthread -ldl

sqlite3_SOURCES = shell.c  sqlite3.c  sqlite3ext.h  sqlite3.h

