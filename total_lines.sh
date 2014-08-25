#!/bin/bash

headerline=`find ./src -name \*.h | xargs cat | wc -l`;
echo "header code in ./src dir = $headerline";

sourceline=`find ./src -name \*.c | xargs cat | wc -l`;
echo "source code in ./src dir = $sourceline";

testhdrline=`find ./libortp/test -name \*.h | xargs cat | wc -l`;
echo "header code in ./libortp/test dir = $testhdrline";
testsrcline=`find ./libortp/test -name \*.c | xargs cat | wc -l`;
echo "source code in ./libortp/test dir = $testsrcline";

sqliteline=`find ./libsqlite/test -name \*.c | xargs cat | wc -l`;
echo "source code in ./libsqlite/test dir = $sqliteline";

totalline=`expr $headerline + $sourceline + $testhdrline \
           + $testsrcline + $sqliteline`;
echo "total code written in project = $totalline";
