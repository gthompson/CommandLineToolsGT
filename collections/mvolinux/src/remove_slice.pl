#!/usr/bin/perl -w
use strict;
use POSIX qw(log10);
use Time::Local;

# Name:           remove_slice
# Source:         /data2/development/ampengfft/remove_slice.pl
# Arguments:      see below
# Returns:        0 on success 1 on failure

# For removing the ampengfft data from an s-file

my $sfile;
my $usage  = "USAGE: remove_slice sfile\n";
my $i;

# Read command-line argument
for ($i=0; $i<@ARGV; $i++){
	if (!$sfile){
		$sfile = $ARGV[$i];
	}
	else{
		die "Extra argument: $usage";
	}
}
unless ($sfile){
	die "Missing argument: $usage";
}

open (IN,"$sfile") || die "Can't open $sfile\n";
open (OUT,">temp_sfile") || die "Can't open temp_sfile\n";
while (<IN>){

	next if (/STAT CMP   MAX AVG  TOTAL ENG             FREQUENCY BINS \(Hz\)       MAX/);
	next if (/AMP \(m\/s\)   \(J\/kg\)   0  1  2  3  4  5  6  7  8  9  10 30  \(Hz\)/);
	next if (/^ VOLC .{8} A.{8} E.{53}3$/);
	next if (/^ trigger window=/);
	
	print OUT;
}
close(OUT);
close(IN);

system ("mv temp_sfile $sfile");
exit(0);


