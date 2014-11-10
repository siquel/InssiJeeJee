#!/usr/bin/perl

use strict;
use warnings;

sub header {
  my $file = shift;
  open my $input, "<", $file or die "Couldn't open '$file':$!";
  my @file = <$input>;
  close $file;
  return toc(@file);
}

sub toc {
  my @lines = @_;
  for( @lines ) {
    if( /^\s*$/s ) { $_ = qq{"\\n"\n}; next; }
    if( /^\s*#/ ) { $_ = qq{"\\n"\n}; next; }
    s/\\/\\\\/g; # double the number of \'s
    s/"/\\"/g;
    s/^\s*/"/;
    s/\n/\\n"\n/;
  }
  return @lines;
}

for my $files (
	[ "api.pm.h",         # output file
		"lib/API.pm",      # input files
	]
) {
	my ($output,@inputs) = @$files;

	open my $header, ">", $output or die "Couldn't open '$output': $!";

	print $header qq["BEGIN {\\n"\n];
	for my $input ( @inputs ) {
		(my $trimmed = $input) =~ s{^lib/}{};
		print $header qq["\$INC{'$trimmed'} = 'Compiled into the plugin.';\\n"\n];
	}
	print $header qq["}\\n"\n];

	for my $input ( @inputs ) {
		print $header qq["{\\n"\n];
		print $header qq{"#line 1 \\"$input\\"\\n"\n};
		print $header header( $input );
		print $header qq["}\\n"\n];
	}
	close $header;
}
