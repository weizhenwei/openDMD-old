#!/bin/bash

echo "aclocal ..."
aclocal
echo "autoheader ..."
autoheader
echo "autoconf ..."
autoconf
echo "automake ..."
automake --add-missing
echo "configure ..."
./configure
