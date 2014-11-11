package InssiJeeJee;
use File::Basename;

sub load {
	my $file = shift @_;
	my $package = file_to_package($file);

	if (open my $handle, $file) {
		# read the file
		my $source = do {
			local $/; 
			<$handle>
		};
		print $source;
		close $handle;
		$source =~ s/^/#start $file\n\x7Bpackage $package;/;
		_do_eval($source);

	} else {
		# file not open
		return 1;
	}
	# all ok
	return 0;
}

sub file_to_package {
	my $f = shift @_;
	$f = File::Basename::basename($f);
	$f =~ s/\.pl$//i;
	return "InssiJeeJee::Plugin::$f";
}

sub _do_eval {
	no strict;
	no warnings;
	eval $_[0];
}

1
