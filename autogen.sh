#!/bin/bash

ACLOCAL=aclocal
AUTOHEADER=autoheader
AUTOCONF=autoconf
LIBTOOL=libtoolize
AUTOMAKE=automake

echo "Running aclocal..."
$ACLOCAL -I m4

echo "Running autoheader..."
$AUTOHEADER

echo "Running autoconf..."
$AUTOCONF

echo "Running libtoolize..."
$LIBTOOL --automake --copy --force

echo "Running automake..."
$AUTOMAKE --force-missing --add-missing --copy

# echo "Regenerate configure file done!"

echo "configure ..."
./configure --prefix=`pwd`/install --enable-debug --disable-gcov

echo "Regenerate configure file done!"

