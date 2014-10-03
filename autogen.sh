#!/bin/bash

ACLOCAL=aclocal
AUTOHEADER=autoheader
AUTOCONF=autoconf
AUTOMAKE=automake

echo "aclocal ..."
$ACLOCAL

echo "autoheader ..."
$AUTOHEADER

echo "autoconf ..."
$AUTOCONF

echo "automake ..."
$AUTOMAKE --force-missing --add-missing

echo "configure ..."
./configure --enable-debug --enable-jit --disable-web --disable-gcov

