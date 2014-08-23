#!/bin/bash

headerline=`find ./src -name \*.h | xargs cat | wc -l`;
echo "header line in ./src dir = $headerline";

sourceline=`find ./src -name \*.c | xargs cat | wc -l`;
echo "source line in ./src dir = $sourceline";

testhdrline=`find ./libortp/test -name \*.h | xargs cat | wc -l`;
echo "header line in ./libortp/test dir = $testhdrline";
testsrcline=`find ./libortp/test -name \*.c | xargs cat | wc -l`;
echo "source line in ./libortp/test dir = $testsrcline";

sqliteline=`find ./libsqlite/test -name \*.c | xargs cat | wc -l`;
echo "source line in ./libsqlite/test dir = $sqliteline";

totalline=`expr $headerline + $sourceline + $testhdrline \
           + $testsrcline + $sqliteline`;
echo "totalline  in ./src dir = $totalline";
