#!/usr/bin/env perl
use strict;
use warnings;
use File::Basename;

my @links;

while (<>) {
  my $dir = dirname ($ARGV);
  $dir = ($dir eq '.') ? '' : $dir . '/';
	if (/(?<!!)\[.+?\]\((.+?)\)/) {
		my $link = $1;
		next if $link =~ /^https?:/;
		next if $link =~ /^file:/;
		next if $link =~ /^#/;
		$dir =~ s/^src\///;
  	push @links, "$dir$link";
  }
}

print 'PAGES+= ';
print join ' ', @links;
print "\n";
