#!/usr/bin/perl 

my $headerline= `find ./src -name \*.h | xargs cat | wc -l`;
print "header line in ./src dir = ".$headerline;

my $sourceline= `find ./src -name \*.c | xargs cat | wc -l`;
print "source line in ./src dir = ".$sourceline;

my $testhdrline = `find ./libortp/test -name \*.h | xargs cat | wc -l`;
print "header line in ./libortp/test dir = ".$testhdrline;
my $testsrcline = `find ./libortp/test -name \*.c | xargs cat | wc -l`;
print "source line in ./libortp/test dir = ".$testsrcline;

my $totalline = $headerline + $sourceline + $testhdrline + $testsrcline;
print "totalline  in ./src dir = ".$totalline;
print "\n";
