#!/usr/bin/perl 

my $headerline= `find ./src -name \*.h | xargs cat | wc -l`;
print "headerline = ".$headerline;

my $sourceline= `find ./src -name \*.c | xargs cat | wc -l`;
print "sourceline = ".$sourceline;

my $totalline = $headerline + $sourceline;
print "totalline  = ".$totalline;
print "\n";
