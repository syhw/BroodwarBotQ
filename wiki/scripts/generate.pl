#!/usr/bin/env perl
use strict;
use warnings;

$^I='.bak';

my $cmd = shift;

my $cmd_out = qx#$cmd#;

while (<>) {
	s/WIKI_MK_PLACEHOLDER/$cmd_out/;
	print;
}

