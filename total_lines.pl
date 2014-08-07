#!/usr/bin/perl 

my $headerline= `find ./src -name \*.h | xargs cat | wc -l`;
print "headerline in ./src dir = ".$headerline;

my $sourceline= `find ./src -name \*.c | xargs cat | wc -l`;
print "sourceline in ./src dir = ".$sourceline;

my $totalline = $headerline + $sourceline;
print "totalline  in ./src dir = ".$totalline;
print "\n";
