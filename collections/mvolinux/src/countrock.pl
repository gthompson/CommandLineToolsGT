#!/usr/bin/perl -w
use strict;
use Time::Local("timegm");

# Name:           countrock
# Source:         /data2/development/ampengfft/countrock.pl
# Calls:          rbuffer2trig, ampengfft
# Arguments:      date range (inclusive)
# Returns:        0 on success 1 on failure

# Finds rockfalls in date range given using s-files
# Similar to compile_slice but instead of using event files
# runs rbuffer2trig on rbuffers to ensure detrigger not premature

# List can be plotted using plotslice

# Configuration
my $debug           = 0;   
my $sfile_path      = "$ENV{EVENT_PATH}";
my @stations        = ("MBGH","MBLY","MBWH","MBHA","MBLG","MBRY","MBFR","MBBY","MBGB");

my $usage = "usage: countrock yyyymmddhh yyyymmddhh (date range inclusive)\n";

# Local variables
my ($tempdir,$event_wav_path);
my ($year1,$month1,$day1,$hour1,$min1,$sec1);
my ($year2,$month2,$day2,$hour2,$min2,$sec2);
my ($year,$month,$day,$hour,$min,$sec,$time);
my ($rocktime,$time1,$time2);
my ($m1,$m2,$d1,$d2,$h1,$h2);
my ($pattern,$line);
my ($sfile,@sfiles);
my ($wavfile,$buffer,$rockfile);
my ($timediff,$mintimediff);
my ($station,$prevtime,$duration);
my (@slices,$slice,$energy,$i);

# Check that everything is mounted
unless (-e $sfile_path){
	die "$sfile_path: path not available.\n";
}

# Command line arguments
die "$usage\n" unless ($ARGV[1]);
die "$usage\n" unless ($ARGV[0] =~ /^(\d\d\d\d)(\d\d)(\d\d)(\d\d)$/);
$year1=$1; $month1=$2; $day1=$3; $hour1=$4;
die "$usage\n" unless ($ARGV[1] =~ /^(\d\d\d\d)(\d\d)(\d\d)(\d\d)$/);
$year2=$1; $month2=$2; $day2=$3; $hour2=$4;

# Open output file
open (OUT,">>countrock.out") || die "Can't open countrock.out";

# Put header - could be half way down file but stations used might change
print OUT "                   ";
print OUT " t-epoch     repose  dur";
foreach $station (@stations){
	print OUT " " x 14;
	print OUT $station;
	print OUT " " x 13;
}
print OUT "\n";

# Create temporary directory for intermediate files
$tempdir = "countrock.$$";
print `mkdir $tempdir`;
die if ($?);
chdir $tempdir || die "Can't cd to $tempdir\n";
print "Working in $tempdir\n" if ($debug);

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

					# Get event type from inside file
					$line = `grep 'VOLC MAIN' $sfile`;
					if ($?==256){
						print "countrock: No VOLC MAIN line in $sfile\n" if ($debug);
						exit(0);
					}
					die "ERROR: count_event: $line\n" if ($?==512);
					unless ($line =~ /^ VOLC MAIN ([a-z]+) /){
						die "ERROR: countrock: Can't read VOLC MAIN line $line\n";
					}

					# Add to array if a rockfall
					if ($1 eq "r" or $1 eq "e"){
						push @sfiles, $sfile;
					}
				}
			}   #hour
		}   # day
	}	# month
}   # year

# Redefine EVENT_WAV_PATH environment variable so ampengfft looks in temp directory
$event_wav_path = $ENV{EVENT_WAV_PATH};
$ENV{EVENT_WAV_PATH} = ".";

foreach $sfile (@sfiles){

	print "Working on $sfile";

	$sfile =~ /(\d\d)\-(\d\d)(\d\d)\-(\d\d).\.S(\d{4})(\d\d)$/;
	$rocktime=timegm($4,$3,$2,$1,$6-1,$5-1900);

	# Look at 20 minutes before event and 20 minutes after it
	$time1 = $rocktime - 1200;
	($sec1,$min1,$hour1,$day1,$month1,$year1) = gmtime $time1;
	$month1 += 1; $year1 += 1900;
	$time2 = $rocktime + 1200;
	($sec2,$min2,$hour2,$day2,$month2,$year2) = gmtime $time2;
	$month2 += 1; $year2 += 1900;

	# rbuffer2trig leaves event files in current temp directory
	$line = sprintf("rbuffer2trig %d%02d%02d%02d%02d %d%02d%02d%02d%02d",
	                 $year1,$month1,$day1,$hour1,$min1,$year2,$month2,$day2,$hour2,$min2);

	if ($debug){
		print `$line`;
	}
	else{
		print `$line 2>1 > /dev/null`;
	}	
	print STDERR "countrock: rbuffer2trig problem\n", next if ($?);

	# Find event corresponding to rockfall - allow 1 minute either way for trigger differences
	$mintimediff = 60;
	$rockfile="";
	foreach $wavfile (`ls 2>/dev/null`){

		next unless ($wavfile =~ /(\d\d\d\d)\-(\d\d)\-(\d\d)\-(\d\d)(\d\d)-(\d\d)/);
		$time1 =timegm($6,$5,$4,$3,$2-1,$1-1900);

		# Within one minute find the closest
		$timediff = abs($time1-$rocktime);

		if ( $timediff < $mintimediff){
			chop ($rockfile = $wavfile);
			$year=$1; $month=$2; $day=$3; $hour=$4; $min=$5; $sec=$6;
			$time=$time1;
			$mintimediff = $timediff;
		}
	}
	unless ($rockfile){
		print "Didn't find event corresponding to $sfile\n";
		next;
	}
	print "Found $rockfile\n";

	# Put event file where ampengfft will look for it
	print `mkdir -p $year/$month`;
	print `mv $rockfile $year/$month`;

	# create mock s-file with type 6 line for ampengfft to read and write to
	open (TEMP,">temp_sfile") || die "Can't open temp_sfile\n";
	printf TEMP " %-77s 6\n", $rockfile;
	close (TEMP);

	# Run ampengfft
	print `ampengfft temp_sfile`;
	die "countrock: ampengfft problem on $year/$month/$rockfile\n" if ($?);

	# Check if s-file has a duration
	$line = `grep window temp_sfile`;
	if ($?==256){
		print "no trigger window in $sfile\n";
		next;
	}
	die "ERROR: countrock: $line\n" if ($?==512);
	$line =~ /trigger window= *(\d+)s/;
	$duration = $1;

	# Write parametric data to file
	printf OUT "%d-%02d-%02d %02d:%02d:%02d",$year,$month,$day,$hour,$min,$sec;
	printf OUT " %d",$time;

	# Time since previous rockfall
	if ($prevtime){
		printf OUT " %7d",$time-$prevtime;
	}
	else{
		print OUT "        ";
	}
	$prevtime=$time;

	printf OUT " %3d", $duration;

	# frequency slices
	for ($i=0; $i<@stations; $i++){
		$line = `grep '$stations[$i] ..Z' temp_sfile`;
		if ($line =~ / E(\d\.\d\de\-\d\d) F(\d+) +(\d+) +(\d+) +(\d+) +(\d+) +(\d+) +(\d+)/){
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
`rm -r $tempdir` unless ($debug);

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





