#!/usr/bin/perl -w
use strict;
use POSIX("mktime");

# Name:           compile_slice
# Source:         /data2/development/ampengfft/compile_slice.pl
# Calls:          ampengfft
# Arguments:      date range (inclusive)
# Returns:        0 on success 1 on failure

#
# Runs ampengfft for every event in given time range 
# Compiles list of events with parameters:
#  time since 1970, time since last event, duration, 
#  energy and % energy in each slice for each station in list.

# List can be plotted using plot_slice

# Configuration
my $debug           = 1;
my $outfile         = "compile_slice.dat";
my $sfile_path      = "$ENV{EVENT_PATH}";
my @stations        = ("MBGH","MBLY","MBWH","MBHA","MBLG","MBRY","MBFR","MBBY","MBGB");

my $usage = "usage: compile_slice [out=outfile] [t1=yyyymmdd[hh][mi][ss]] [t2=yyyymmdd[hh][mi][ss]] (date range inclusive)\n";

# Local variables
my ($tempdir);
my ($year1,$month1,$day1,$hour1,$min1,$sec1);
my ($year2,$month2,$day2,$hour2,$min2,$sec2);
my ($year,$month,$day,$hour,$min,$sec,$eventtime);
my ($m1,$m2,$d1,$d2,$h1,$h2);
my ($pattern,$line,$event_type);
my ($sfile,@sfiles);
my ($station,$prevtime,$duration);
my (@slices,$slice,$energy,$i);

# Check that everything is mounted
unless (-e $sfile_path){
	die "$sfile_path: path not available.\n";
}

# Command line arguments
for ($i=0; $i<@ARGV; $i++){

	if  ($ARGV[$i] =~ /^out=(.+)$/){
		$outfile = $1;
	}
	elsif ($ARGV[$i] =~ /^t1=(\d\d\d\d)(\d\d)(\d\d)(\d\d)?(\d\d)?(\d\d)?$/){
		$hour1=0; $min1=0; $sec1=0;
		$year1=$1; $month1=$2; $day1=$3;
		$hour1=$4 if ($4);
		$min1=$5  if ($5);
		$sec1=$6  if ($6);
	}
	elsif ($ARGV[$i] =~ /^t2=(\d\d\d\d)(\d\d)(\d\d)(\d\d)?(\d\d)?(\d\d)?$/){
		$hour2=0; $min2=0; $sec2=0;
 		$year2=$1; $month2=$2; $day2=$3;
		$hour2=$4 if ($4);
		$min2=$5  if ($5);
		$sec2=$6  if ($6);
	}
	else{
		die "Unrecognised argument $ARGV[$i]\n$usage\n";
	}
}

# Default to now if no second time given
($sec2,$min2,$hour2,$day2,$month2,$year2) = gmtime();
$month2++;
$year2+=1900;

# Check stations haven't changed
if (-e $outfile){

	$line = `head -1 $outfile`;	

	unless ($line =~ s/^                       t-epoch     repose  dur//){
		die "header doesn't match pattern: $line\n";
	}
	$i=0;
	while ($line =~ s/^ +(MB..)//){
		if ($1 ne $stations[$i++]){
			print STDERR "$1 vs $stations[$i]\n";
			die "Station list not consistent: want new output file\n";
		}
	}
}
# If no t1 given then read last line of file if it exists already
unless ($year1){

	unless (-e $outfile){
		die "Need t1 if outfile doesn't exist yet\n $usage\n";
	}

	$line = `tail -1 $outfile`;	

	unless ($line =~ /^(\d\d\d\d)\-(\d\d)\-(\d\d) (\d\d):(\d\d):(\d\d) \w  ([ \d]{10})/){
		die "last line doesn't match pattern: $line\n";
	}
	if ($5 eq "       " or $6 eq "   "){
		die "last line doesn't match pattern: $1\n";
	}
 	$year1=$1; $month1=$2; $day1=$3; $hour1=$4; $min1=$5; $sec1=$6;

	$prevtime=$7*1;

	# Open output file
	open (OUT,">>$outfile") || die "Can't open $outfile";
}
# Only want header if new file
else{ 
	# Open output file
	open (OUT,">>$outfile") || die "Can't open $outfile";
	
	print OUT "                   ";
	print OUT "    t-epoch     repose  dur";
	foreach $station (@stations){
		print OUT " " x 14;
		print OUT $station;
		print OUT " " x 13;
	}
	print OUT "\n";
}

# Create temporary directory for intermediate files
$tempdir = "compile_slice.$$";
print `mkdir $tempdir`;
die if ($?);
chdir $tempdir || die "Can't cd to $tempdir\n";
print "Working in $tempdir\n" if ($debug);

if ($debug){
	print "$year1$month1$day1$hour1$min1$sec1 to $year2$month2$day2$hour2$min2$sec2\n";
}
# Look for s-files for requested time range
for ($year=$year1; $year<=$year2; $year++){
	if ($year == $year1){ $m1 = $month1; }
	else { $m1 = 1; }
	if ($year == $year2){ $m2 = $month2; }
	else { $m2 = 12; }
	
	for ($month=$m1; $month<=$m2; $month++){
		if ($year == $year1 && $month == $month1){ $d1 = $day1; }
		else { $d1 = 1; }
		if ($year == $year2 && $month == $month2){ $d2 = $day2; }
		else { $d2 = days_in_month($month,$year); }
		
		for ($day=$d1; $day<=$d2; $day++){
			if ($year==$year1 && $month==$month1 && $day==$day1){ $h1 = $hour1; }
			else { $h1 = 0; }
			if ($year==$year2 && $month==$month2 && $day==$day2){ $h2 = $hour2; }
			else { $h2 = 23; }

			for ($hour=$h1; $hour<=$h2; $hour++){
				# Loop over events in given hour
				$pattern  = sprintf "$sfile_path/%d/%02d/",$year,$month;
				$pattern .= sprintf "%02d-%02d??-??L\.S%4d%02d",$day,$hour,$year,$month;
				foreach $sfile (`ls $pattern 2>/dev/null`){

					# Check within first hour whether want this one or not
					if ($hour==$h1){
						$sfile =~ /(\d\d)\-(\d\d)L\.S\d\d\d\d\d\d/;
						next if ($1<$min1);
						next if ($1==$min1 && $2<=$sec1);
					}

					# Check within last hour whether want this one or not
					if ($hour==$h2){
						$sfile =~ /(\d\d)\-(\d\d)L\.S\d\d\d\d\d\d/;
						next if ($1>$min2);
						next if ($1==$min2 && $2>=$sec2);
					}
					
					# Only want broadband network events
					$line = `grep MVO $sfile`;
					next if ($?==256);
					die "ERROR: compile_slice: $line\n" if ($?==512);
					
					push @sfiles, $sfile;
				}
			}   #hour
		}   # day
	}	# month
}   # year

foreach $sfile (@sfiles){

	print "Working on $sfile";

	# Get event type from inside file
	$line = `grep 'VOLC MAIN' $sfile`;
	if ($?==256){
		print "WARNING: compile_slice: No VOLC MAIN line in $sfile\n";
		next;
	}
	die "ERROR: compile_slice: $line\n" if ($?==512);
	unless ($line =~ /^ VOLC MAIN ([a-z]+) /){
		die "ERROR: compile_slice: Can't read VOLC MAIN line $line\n";
	}
	$event_type=$1;
	
	next if ($event_type eq "n");
					
	$sfile =~ /(\d\d)\-(\d\d)(\d\d)\-(\d\d).\.S(\d{4})(\d\d)$/;
	$eventtime=mktime($4,$3,$2,$1,$6-1,$5-1900);
	$day=$1; $hour=$2; $min=$3; $sec=$4; $year=$5; $month=$6;
	
	# create mock s-file with type 6 line for ampengfft to read and write to
	# do not include short period event files in temporary file
	chomp $sfile;
	open (IN,"$sfile") || die "Can't open $sfile\n";
	open (TEMP,">temp_sfile") || die "Can't open tem_sfile\n";
	while (<IN>){
		next if (/SPN/);
		print TEMP $_;
	}
	close IN;
	close TEMP;
	 
	# Run ampengfft unless ampengfft data already in sfile
	$line = `grep "      STAT CMP   MAX" temp_sfile`;	
	if ($?==256){
		$line = `ampengfft temp_sfile 2>&1`;
	}
	
	# Check if s-file has a duration
	$line = `grep window temp_sfile`;
	if ($?==256){
		print "no trigger window in $sfile\n";
		next;
	}
	die "ERROR: compile_slice: $line\n" if ($?==512);
	$line =~ /trigger window= *(\d+)s/;
	$duration = $1;

	# Write parametric data to file
	printf OUT "%d-%02d-%02d %02d:%02d:%02d",$year,$month,$day,$hour,$min,$sec;
	
	printf OUT " $event_type ";
	
	# Time since epoch
	printf OUT " %10d",$eventtime;
	
	# Time since previous event
	if ($prevtime){
		printf OUT " %7d",$eventtime-$prevtime;
	}
	else{
		print OUT "        ";
	}
	$prevtime=$eventtime;
	
	printf OUT " %3d", $duration;

	# frequency slices
	for ($i=0; $i<@stations; $i++){
		$line = `grep '$stations[$i] ..Z' temp_sfile`;
		if ($line =~ / E(\d\.\d\de\-\d\d) F ?(\d+) +(\d+) +(\d+) +(\d+) +(\d+) +(\d+) +(\d+)/){
			$energy=$1; $slices[0]=$2; $slices[1]=$3; $slices[2]=$4; $slices[3]=$5; $slices[4]=$6; $slices[5]=$7; $slices[6]=$8;
		}
		else{
			$energy=0;
		}
		if ($energy){
			printf OUT "  $energy";
			foreach $slice (@slices){
				printf OUT " %2d",$slice;
			}
		}
		else{
			printf OUT " " x 31;
		}

	
	}
	print OUT "\n";
}
close(OUT);

# remove temporary directory
chdir "..";
`rm -r $tempdir`;

exit(0);


################################################################################

# Name:         days_in_month
# Arguments:    month and year
# Returns:      number of days in the given month for that year

sub days_in_month {
	my ($mm,$yyyy) = @_;
	my @dayspermonth = (0,31,28,31,30,31,30,31,31,30,31,30,31);

	$dayspermonth[2]=29 unless ($yyyy % 4);

	return $dayspermonth[$mm];
}





