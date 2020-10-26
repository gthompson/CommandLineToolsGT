#!/usr/bin/perl -w
use strict;
use POSIX qw(log10);
use Time::Local;
use POSIX qw(mktime);

# Name:           plot_sfile
# Source:         /data2/development/ampengfft/plot_sfile.pl
# Arguments:      see below
# Returns:        0 on success 1 on failure

# For plotting the ampengfft data direct from s-files
# Would normaly use compile_slice and then plot_slice
# This is much slower but can add extras such as max_freq 

# Set default parameters, can be replaced from command line
my $title      = " ";                 # default title of plot
my $psfile     = "plot_sfile.ps";      # name of resulting ps file
my $types      = "er";                # event type codes concattenated together
my $xsta       = "MBGH";
my $xcom       = "BHZ";
my $ysta       = "MBWH";
my $ycom       = "BHZ";
my $debug      = 1;

# If plotting depth exclude depths that are very poorly known
my $max_deperr = 1;

# If plotting peak amplitude frequency exclude outliers to make bins smaller
my $max_freq = 5;

# These shouldn't change
my $nslice     = 12;
my $sfile_path = "$ENV{EVENT_PATH}";

# Get spurious tiny energy values sometimes if trace dead
my %discard_if_less = (   "MBGH" => 1.00e-10,
                          "MBLY" => 1.00e-10,
                          "MBWH" => 1.00e-13,
                          "MBHA" => 1.00e-10,
                          "MBLG" => 1.00e-9,
                          "MBRY" => 1.00e-11,
                          "MBFR" => 1.00e-11,
                          "MBBY" => 1.00e-11,
                          "MBGB" => 1.00e-13  );
						  
#  Get spurious huge energy values if trace goes to rail during event
my %discard_if_more   = ( "MBGH" => 1.00e-5,
                          "MBLY" => 1.00e-6,
                          "MBWH" => 3.00e-6,
                          "MBHA" => 1.00e-6,
                          "MBLG" => 1.00e-3,
                          "MBRY" => 1.00e-6,
                          "MBFR" => 1.00e-6,
                          "MBBY" => 1.00e-6,
                          "MBGB" => 1.00e-6  );						  
# Plot params
my $xoffset = 3.4;
my $yoffset = 2;
my $xaxlen = 17;
my $yaxlen = 23;
my $symbol = "x0.3";
my $shade = "100/200/100";        # nasty greeny thing
my $xlabel = "";
my $ylabel = "";
my $numxtick = 15;
my $numytick = 15;

# Script starts here
my ($usage);
my ($data,$gmtcom,$line);
my ($xslice1,$xslice2,$yslice1,$yslice2);
my ($year1,$month1,$day1,$hour1);
my ($year2,$month2,$day2,$hour2);
my ($sec,$min,$hour,$day,$month,$year);
my ($m1,$m2,$d1,$d2,$h1,$h2);
my (@datetime,@day,@day_label);
my ($xmaxfac,$ymaxfac);
my ($fac,@slice);
my ($labely,$labelx);
my ($epoch,$prev_epoch,$epoch_start);
my ($xpar,$ypar,@x,@y);
my ($xmin,$xmax,$ymin,$ymax);
my ($numday,$time_interval,$day_label,$day_start);
my ($xmaxtime,$xmintime,$ymaxtime,$ymintime);
my ($xtick,$ytick,$bin);
my ($yyyy,$mm,$dd,$hh,$mi,$ss);
my ($pattern,$sfile);
my ($cumtot);
my ($i,$j);
my ($num);

$usage  = "USAGE: plot_sfile t1=yyyymmddhh [t2=yyyymmddhh] [title=] [types=] ";
$usage .= "[xsta=] [xcom=] [ysta=] [ycom=]\n";
$usage .= "x=(epoch|rep|dur|eng|logeng|slice\\d\\d]|dep|peak)\n";
$usage .= "y=(num|epoch|rep|dur|eng|logeng||cumeng|dayeng|slice\\d\\d|dep|peak)\n";

# Read command-line options
for ($i=0; $i<@ARGV; $i++){

	if ($ARGV[$i] =~ /^t1=(\d\d\d\d)(\d\d)(\d\d)(\d\d)?$/){
		$year1=$1; $month1=$2; $day1=$3; $hour1=0; $hour1=$4 if ($4);
	}
	elsif ($ARGV[$i] =~ /^t2=(\d\d\d\d)(\d\d)(\d\d)(\d\d)?$/){
		$year2=$1; $month2=$2; $day2=$3; $hour2=0; $hour2=$4 if ($4);
	}
	elsif ($ARGV[$i] =~ /^xsta=([A-Z]+)$/){
		$xsta=$1;
	}
	elsif ($ARGV[$i] =~ /^xcom=([A-Z]+)$/){
		$xcom=$1;
	}
	elsif ($ARGV[$i] =~ /^ysta=([A-Z]+)$/){
		$ysta=$1;
	}
	elsif ($ARGV[$i] =~ /^ycom=([A-Z]+)$/){
		$ycom=$1;
	}
	elsif ($ARGV[$i] =~ /^types=(\w+)$/){
		$types = $1;
	}
	elsif ($ARGV[$i] =~ /x=(epoch|rep|dur|eng|logeng|slice\d\d|dep|peak)$/){
		$xpar = $1;
		if ($xpar =~ /^slice(\d)(\d)/){
			$xpar = "slice";
			$xslice1 = $1;
			$xslice2 = $2;
			die "Bad x argument $ARGV[$i]\n" if ($xslice1<1 or $xslice1>7);
			die "Bad x argument $ARGV[$i]\n" if ($xslice2<1 or $xslice2>7);
		}
	}
	elsif ($ARGV[$i] =~ /y=(num|epoch|rep|dur|eng|logeng|dayeng|cumeng|slice\d\d|dep|peak)$/){
		$ypar = $1;
		if ($ypar =~ /^slice(\d)(\d)/){
			$ypar = "slice";
			$yslice1 = $1;
			$yslice2 = $2;
			die "Bad x argument $ARGV[$i]\n" if ($yslice1<1 or $yslice1>7);
			die "Bad x argument $ARGV[$i]\n" if ($yslice2<1 or $yslice2>7);
		}
	}
	elsif ($ARGV[$i] =~ /title=(.+)/){
		$title = $1;
	}
	else {
		print STDERR "Unrecognised argument $i: $ARGV[$i].\n";
		die "$usage\n";
	}
}
unless ($xpar && $ypar){
	die "Missing argument $usage\n";
}
if ($ypar eq "cumeng" && $xpar ne "epoch"){
	die "Cumulative energy only makes sense plotted against epoch\n";
}
if ($ypar eq "dayeng" && $xpar ne "epoch"){
	die "Daily energy only makes sense plotted against epoch\n";
}
$title = "types=$types" if ($title eq " ");

unless ($year2){
	($sec,$min,$hour2,$day2,$month2,$year2) = localtime() ;
	$year2 += 1900;
	$month2++;
}

printf ("Types $types between %d%02d%02d%02d ",$year1,$month1,$day1,$hour1);
printf ("and %d%02d%02d%02d\n",$year2,$month2,$day2,$hour2);

# Plot energy multiplied by exponential factor
$xmaxfac = $ymaxfac = 0;

# Look for s-files for requested time range
$i=0;
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
		$x[$i] = $y[$i] = 0;
		chomp($sfile);

		# Get event type from inside file
		$line = `grep 'VOLC MAIN' $sfile`;
		if ($?==256){
			print "$sfile: No VOLC MAIN line\n" if ($debug);
			next;
		}
		die "ERROR: plot_sfile: $sfile: grep VOLC MAIN\n" if ($?==512);
		unless ($line =~ /^ VOLC MAIN ([a-z]+) /){
			die "ERROR: plot_sfile: $sfile: Can't read VOLC MAIN line\n";
		}
		next if ($types !~ /$1/);
				
		$sfile =~ /\d\d\-\d\d(\d\d)\-(\d\d)/;
		$min=$1; $sec=$2;
		$datetime[$i] = sprintf ("%4d-%02d-%02d %02d:%02d:%02d",$year,$month,$day,$hour,$min,$sec);

		if ($xpar =~ /(epoch|rep)/ or $ypar =~ /(epoch|rep)/){

			$epoch=mktime($sec,$min,$hour,$day,$month-1,$year-1900);
			if ($i==0){
				($ss,$mi,$hh,$dd,$mm,$yyyy) = localtime($epoch);
				$day_start = timelocal(0,0,0,$dd,$mm,$yyyy);
				$epoch_start = $epoch;
			}
			else{
				$x[$i] = $epoch-$prev_epoch if ($xpar eq "rep");
				$y[$i] = $epoch-$prev_epoch if ($ypar eq "rep");
			}
			$x[$i] = ($epoch - $day_start)/86400 if ($xpar eq "epoch");
			$y[$i] = ($epoch - $day_start)/86400 if ($ypar eq "epoch");
			$prev_epoch = $epoch;
		}

		# Depth, if required 
		if ($xpar eq "dep" or $ypar eq "dep"){
					
			# Check depth error
			$line = `grep GAP $sfile`;
			next unless ($line =~ /GAP=[\d\. ]{33} +(\d+\.\d)/);	
			next if ($1 > $max_deperr);
			
			$line = `head -1 $sfile`;
			next  unless ($line =~ /-62.\d\d\d +(\d+\.\d)/);
			$x[$i] =$1 if ($xpar eq "dep");
			$y[$i] =$1 if ($ypar eq "dep");
		}
					
		# Duration, if required 
		if ($xpar eq "dur" or $ypar eq "dur"){
			$line = `grep window $sfile`;
			if ($?==256){
				print "no trigger window in $sfile\n";
				next;
			}
			die "ERROR: plot_sfile: $sfile: grep window\n" if ($?==512);
			$line =~ /trigger window= *(\d+)s/;
			$x[$i] = $1;
		}

		# Energy, slices or peak frequency if required
		if ($xpar =~ /(eng|slice|peak)/ ){
			$line = `grep \"VOLC $xsta $xcom\" $sfile`;
			if ($?==256){
				print "$sfile: no line for $xsta $xcom\n";
				next;
			}
			die "ERROR: plot_sfile: $sfile: grep VOLC $xsta $xcom\n" if ($?==512);
			if ($line =~ s/^.{14} A\d\.\d\de\-\d\d E(\d\.\d\de\-(\d\d)) F //){
				if ($1 < $discard_if_less{$xsta} or $1 > $discard_if_more{$xsta}){
					print "$sfile: discarding $1 value for $xsta\n";
					next;
				}
				$x[$i] = $1 if ($xpar eq "eng");
				$x[$i] = POSIX::log10($1) if ($xpar eq "logeng");
				$xmaxfac = $2 if ($2 > $xmaxfac);

				for ($j=0; $j<$nslice; $j++){
			 		$line =~ s/(\d+) +//;
					$slice[$j]=$1*1;
				}
				if ($xpar eq "slice"){
					$x[$i] = 0;
					for ($j=$xslice1; $j<=$xslice2; $j++){
						if ($slice[$j]){
							$x[$i] += $slice[$j];
						}
					}
				}
				unless ($line =~ /^(\d+\.\d\d) 3$/){
					die "ERROR: plot_sfile: $sfile: trouble parsing $xsta $xcom\n";
				}
				if ($xpar eq "peak"){
					next if ($1 > $max_freq);
					$x[$i] = $1;
				}
			}
		}
		if ($ypar =~ /(eng|slice|peak)/ ){
			$line = `grep \"VOLC $ysta $ycom\" $sfile`;
			if ($?==256){
				print "no line for $ysta $ycom in $sfile\n";
				next;
			}
			die "ERROR: plot_sfile: $sfile: grep VOLC $ysta $ycom\n" if ($?==512);

			if ($line =~ s/^.{14} A\d\.\d\de\-\d\d E(\d\.\d\de\-(\d\d)) F //){
				if ($1 < $discard_if_less{$ysta} or $1 > $discard_if_more{$ysta}){
					print "$sfile: discarding $1 value for $ysta\n";
					next;
				}
				$y[$i] = $1 if ($ypar eq "eng" || $ypar eq "cumeng");
				$y[$i] = POSIX::log10($1) if ($ypar eq "logeng");
				$ymaxfac = $2 if ($2 > $ymaxfac);

				for ($j=0; $j<$nslice; $j++){
			 		$line =~ s/(\d+) +//;
					$slice[$j]=$1*1;
				}
				if ($ypar eq "slice"){
					$y[$i] = 0;
					for ($j=$yslice1; $j<=$yslice2; $j++){
						if ($slice[$j]){
							$y[$i] += $slice[$j];
						}
					}
				}
				unless ($line =~ /^(\d+\.\d\d) 3$/){
					die "ERROR: plot_sfile: $sfile: trouble parsing $ysta $ycom: $line\n";
				}
				if ($ypar eq "peak"){
					next if ($1 > $max_freq);
					$y[$i] = $1;
				}
			}
		}
		$i++;
	} # foreach file within this hour
   } # hour
  } # day
 } # month
} # year
$num=$i;

die "No data found\n" if ($num==0);

if ($xpar eq "epoch"){
    $xlabel="Day";
}
elsif ($xpar eq "rep"){
    $xlabel="Repose Time (sec)";
}
elsif ($xpar eq "dep"){
    $xlabel="Depth (km)";
}
elsif ($xpar eq "dur"){
	$xlabel="Duration (sec)";
}
elsif ($xpar eq "logeng"){
	$xlabel="Log energy on $xsta";
}
elsif ($xpar eq "peak"){
	$xlabel="Maximum amplitude frequency on $xsta";
}
elsif ($xpar eq "eng"){
	for ($i=0; $i<$num; $i++){
		next unless ($x[$i]);
		$x[$i] *= "1e$xmaxfac";
	}
	$xlabel="Energy on $xsta] (e-$xmaxfac) J/kg";
}
elsif ($xpar eq "slice"){
	$xlabel  = "% Energy between ";
	$xlabel .= $xslice1-1;
	$xlabel .= " and $xslice2 Hz on $xsta";
}


if ($ypar eq "epoch"){
    $ylabel="Days since $day[0]";
}
elsif ($ypar eq "rep"){
    $ylabel="Repose Time (sec)";
}
elsif ($ypar eq "dep"){
    $ylabel="Depth (km)";
}
elsif ($ypar eq "dur"){
	$ylabel="Duration (sec)";
}
elsif ($ypar eq "logeng"){
	$ylabel="Log10 energy on $ysta";
}
elsif ($ypar eq "peak"){
	$ylabel="Maximum amplitude frequency on $ysta";
}
elsif ($ypar eq "eng"){   
	for ($i=0; $i<$num; $i++){
		next unless ($y[$i]);
		$y[$i] *= "1e$ymaxfac";
	}
	$ylabel="Energy on $ysta (e-$ymaxfac) J/kg";
}
elsif ($ypar eq "cumeng"){
	for ($i=0; $i<$num; $i++){
		next unless ($y[$i]);
		$y[$i] *= "1e$ymaxfac";
	}
	$ylabel="Cumulative energy on $ysta (e-$ymaxfac) J/kg";
}
elsif ($ypar eq "slice"){
	$ylabel  = "% Energy between ";
	$ylabel .= $yslice1-1;
	$ylabel .= " and $yslice2 Hz on $ysta";
}

# Set GMT plotting defaults
$gmtcom  = "gmtset MEASURE_UNIT cm LABEL_FONT_SIZE 18 HEADER_FONT_SIZE 24 ";
$gmtcom .= "ANOT_FONT_SIZE 12 GRID_CROSS_SIZE 0.15 DEGREE_FORMAT 1";
`$gmtcom`;
	
# Create string to pass to GMT
# If y parameter given as num then want a histogram
if ($ypar eq "num"){
	$ylabel = "Number";
	$data="";
	$xmax=-10e20; $xmin=10e20;
	for ($i=0; $i<$num; $i++){
		next unless ($x[$i]);
		$data .= "$x[$i]\n";
		$xmax = $x[$i] if ($x[$i]>$xmax);
		$xmin = $x[$i] if ($x[$i]<$xmin);
	}
	$bin = int(($xmax-$xmin)/20);
	$bin = int(($xmax-$xmin)/2)/10 if ($bin==0);
	$bin = 0.1 if ($xpar eq "peak");
	print "fac=$xmaxfac " if ($xpar eq "eng");
	printf ("num=$num xmin=%.2f xmax=%.2f\n",$xmin,$xmax);

	# Need to do the psinfo thing twice as start of binning changes
	$gmtcom  = "pshistogram -JX$xaxlen/$yaxlen -W$bin -C -V -I 2> psinfo";
	open (GMTCOM,"| $gmtcom") || die "Cannot open GMT command\n";
	print GMTCOM $data;
	close (GMTCOM);

	open (MINMAX, "psinfo") or die "Can't open psinfo";
	while (<MINMAX>){
		unless (/are\s:\s(\-?[\d\.]+)\s(\-?[\d\.]+)\s(\-?[\d\.]+)\s(\-?[\d\.]+)/){
			next;
		}
		$xmin = $1; $xmax = $2; $ymin = $3; $ymax = $4;	
		last;
	}
	close(MINMAX);
	`rm psinfo`;

	if ($bin == int($bin)){
		$xmin = $bin * int($xmin/$bin) - 2*$bin;	
		$xmax = $bin * int($xmax/$bin) + 2*$bin;
	}
	else{
		$xmin = $bin * $xmin/$bin - 2*$bin;	
		$xmax = $bin * $xmax/$bin + 2*$bin;
	}

	$gmtcom  = "pshistogram -JX$xaxlen/$yaxlen -R$xmin/$xmax/$ymin/$ymax -W$bin -C -V -I 2> psinfo";
	open (GMTCOM,"| $gmtcom") || die "Cannot open GMT command\n";
	print GMTCOM $data;
	close (GMTCOM);

	open (MINMAX, "psinfo") or die "Can't open psinfo";
	while (<MINMAX>){
		unless (/are\s:\s(\-?[\d\.]+)\s(\-?[\d\.]+)\s(\-?[\d\.]+)\s(\-?[\d\.]+)/){
			next;
		}
		$xmin = $1; $xmax = $2; $ymin = $3; $ymax = $4;	
		last;
	}
	close(MINMAX);
	`rm psinfo`;

	if ($bin == int($bin)){
		$xmin = $bin * int($xmin/$bin) - $bin;	
		$xmax = $bin * int($xmax/$bin) + 2*$bin;
	}
	else{
		$xmin = $bin * $xmin/$bin - $bin;	
		$xmax = $bin * $xmax/$bin + 2*$bin;
	}
	
	$xtick = $bin*4;
	$xlabel = "$xlabel (bin=$bin)";
	$ymax += 1;
	$ytick = int(($ymax-$ymin)/$numytick + 0.5);
	
	printf ("bin=$bin xmin=%.2f xmax=%.2f ymin=%.2f ymax=%.2f\n",$xmin,$xmax,$ymin,$ymax);

	$gmtcom  = "pshistogram -JX$xaxlen/$yaxlen -W$bin -C -L -G$shade ";
	$gmtcom .= "-R$xmin/$xmax/$ymin/$ymax -X$xoffset -Y$yoffset -P -K ";
	$gmtcom .= "-B${xtick}:\"$xlabel\":/${ytick}:\"$ylabel\"::.\"$title\":WSen ";
	$gmtcom .= "> $psfile\n";
	open (GMTCOM,"| $gmtcom") || die "Cannot open GMT command\n";
	print GMTCOM $data;
	close (GMTCOM);
	
}
# If y parameter is dayeng then want a bar chart 
# - also want to output data so don't use histogram
# x must be epoch in this case
elsif ($ypar eq "dayeng"){
	$ylabel="Energy per day on $ysta (e-$ymaxfac)";

	# Need to fix ntick to make room for day labels
	$numxtick = 5;
	
	for ($i=0; $i<$num; $i++){
		next unless ($y[$i]);
		$y[int($x[$i])] += $y[$i] * "1e$ymaxfac";
	}
	$numday = @y;
	$ymax=-10e20; $ymin=10e20;
	for ($i=0; $i<$numday; $i++){
		next unless ($y[$i]);
		$data .= "$i,$y[$i]\n";
		$xmin = $i unless (defined $xmin);
		$ymin = $y[$i] if ($y[$i]<$ymin);
		$ymax = $y[$i] if ($y[$i]>$ymax);
		
		# print data to stdout for TC
		($sec,$min,$hour,$day,$month,$year) = localtime($epoch_start+$i*86400);
		$year += 1900; $month++;
		print "$day/$month/$year,$y[$i]\n";
	}
	$xmax = $numday-1;
	print "yfac=$ymaxfac ";
	printf ("num=%d xmin=%.2f xmax=%.2f ymin=%.2f ymax=%.2f\n",$numday,$xmin,$xmax,$ymin,$ymax);
	
	# Plot axes
	$xmin = int ($xmin) - 1;
	$xmax = int ($xmax) + 1;
	$ymin  = 0;
	$ymax += int (1 + ($ymax-$ymin) / 50);
	$xtick = int (($xmax-$xmin) / $numxtick);
	$ytick = int (($ymax-$ymin) / $numytick);
	$xtick = 1 unless ($xtick);
	$ytick = 1 unless ($ytick);
	$gmtcom =  "psxy /dev/null -JX$xaxlen/$yaxlen -R$xmin/$xmax/$ymin/$ymax ";
	$gmtcom .= "-B${xtick}\/${ytick}:\"$ylabel\"::.\"$title\":Wsen ";
	$gmtcom .= "-X$xoffset -Y$yoffset -P -K > $psfile";
	`$gmtcom`;

	# Plot symbols
	$gmtcom  = "psxy -JX$xaxlen/$yaxlen -R -Sb1u -O -K >> $psfile";
	open (GMTCOM,"| $gmtcom") || die "Cannot open GMT command\n";
	print GMTCOM $data;
	close (GMTCOM);
	
	# Add meaningful labels for epoch
	$time_interval = int($numday * $xtick/($xmax-$xmin) + 0.5);
	$labely = $ymin - ($ymax-$ymin)*0.025;
	$gmtcom  = "pstext -JX$xaxlen/$yaxlen -R$xmin/$xmax/$ymin/$ymax -N -O <<END>> $psfile\n";
	for ($i=0; $i<=$numxtick; $i++){
		$labelx = $xmin + 1 + $i*$xtick;
		($sec,$min,$hour,$day,$month,$year) = localtime($day_start+$time_interval*$i*86400);
		$day_label = sprintf("%d-%02d-%02d",$year+1900,$month+1,$day);
		$gmtcom .= "$labelx $labely 12 0 0 BC $day_label\n";
	}
	$labely = $ymin - ($ymax-$ymin)*0.06;
	$labelx = $xmin + ($xmax-$xmin)/2;
	$gmtcom .= "$labelx $labely 18 0 1 BC $xlabel\n";
	$gmtcom .= "END\n";
	`$gmtcom`;
}
# If y parameter is cumeng then want a line - x is epoch in this case
elsif ($ypar eq "cumeng"){
	$data="";

	# Need to fix ntick to make room for day labels
	$numxtick = 5;

	$cumtot=0;
	$ymin = 0;
	for ($i=0; $i<$num; $i++){
		next unless ($x[$i]);
		next unless ($y[$i]);
		$ymin = $y[$i], $ymintime=$datetime[$i] unless ($ymin);	

		$cumtot += $y[$i];
		$data .= "$x[$i] $cumtot\n";
	}
	$xmin = $x[0], $xmintime=$datetime[0];
	$xmax = $x[$num-1], $xmaxtime=$datetime[$num-1];
	$ymax = $cumtot, $ymaxtime=$datetime[$num-1];

	print "yfac=$ymaxfac ";
	printf ("num=%d xmin=%.2f xmax=%.2f ymin=%.2f ymax=%.2f\n",$num,$xmin,$xmax,$ymin,$ymax);
	print "min x $xmintime\n";
	print "max x $xmaxtime\n";
	print "min y $ymintime\n";
	print "max y $ymaxtime\n";
	
	# Plot axes
	$xmin = int ($xmin) - 1;
	$xmax = int ($xmax) + 2;
	$ymin -= int (1 + ($ymax-$ymin) / 50);
	$ymax += int (1 + ($ymax-$ymin) / 50);
	$xtick = int (($xmax-$xmin) / $numxtick + 0.5);
	$ytick = int (($ymax-$ymin) / $numytick + 0.5);
	$xtick = 1 unless ($xtick);
	$ytick = 1 unless ($ytick);
	$gmtcom =  "psxy /dev/null -JX$xaxlen/$yaxlen -R$xmin/$xmax/$ymin/$ymax ";
	$gmtcom .= "-B${xtick}:\"$xlabel\":\/${ytick}:\"$ylabel\"::.\"$title\":Wsen ";
	$gmtcom .= "-X$xoffset -Y$yoffset -P -K > $psfile";
	`$gmtcom`;

	# Plot symbols
	$gmtcom  = "psxy -JX$xaxlen/$yaxlen -R -O -K >> $psfile";
	open (GMTCOM,"| $gmtcom") || die "Cannot open GMT command\n";
	print GMTCOM $data;
	close (GMTCOM);
	
	# Add meaningful labels for epoch
	$numday = $xmax - $xmin;
	$time_interval = int($numday * $xtick/($xmax-$xmin) + 0.5);
	$labely = $ymin - ($ymax-$ymin)*0.025;
	$gmtcom  = "pstext -JX$xaxlen/$yaxlen -R$xmin/$xmax/$ymin/$ymax -N -O <<END>> $psfile\n";

	for ($i=0; $i<$numxtick; $i++){
		$labelx = $xmin + 1 + $i*$xtick;
		($sec,$min,$hour,$day,$month,$year) = localtime($day_start+$time_interval*$i*86400);
		$day_label = sprintf("%d-%02d-%02d",$year+1900,$month+1,$day);
		$gmtcom .= "$labelx $labely 12 0 0 BC $day_label\n";
	}
	$labely = $ymin - ($ymax-$ymin)*0.06;
	$labelx = $xmin + ($xmax-$xmin)/2;
	$gmtcom .= "$labelx $labely 18 0 0 BC $xlabel\n";
	$gmtcom .= "END\n";
	`$gmtcom`;
}	
# Otherwise want a scatter plot
else{
	$data="";
	$xmax=-10e20; $xmin=10e20;
	$ymax=-10e20; $ymin=10e20;
	for ($i=0; $i<$num; $i++){
		next unless ($x[$i]);
		next unless ($y[$i]);
		$data .= "$x[$i] $y[$i]\n";
		$xmin = $x[$i], $xmintime=$datetime[$i] if ($x[$i]<$xmin);
		$xmax = $x[$i], $xmaxtime=$datetime[$i] if ($x[$i]>$xmax);
		$ymin = $y[$i], $ymintime=$datetime[$i] if ($y[$i]<$ymin);	
		$ymax = $y[$i], $ymaxtime=$datetime[$i] if ($y[$i]>$ymax);
	}
	print "xfac=$xmaxfac " if ($xpar eq "eng");
	print "yfac=$ymaxfac " if ($ypar eq "eng");
	printf ("num=%d xmin=%.2f xmax=%.2f ymin=%.2f ymax=%.2f\n",$num,$xmin,$xmax,$ymin,$ymax);
	print "min x $xmintime\n";
	print "max x $xmaxtime\n";
	print "min y $ymintime\n";
	print "max y $ymaxtime\n";
	
	# Plot axes
	$xmin -= int (1 + ($xmax-$xmin) / 50);
	$xmax += int (1 + ($xmax-$xmin) / 50);
	$ymin -= int (1 + ($ymax-$ymin) / 50);
	$ymax += int (1 + ($ymax-$ymin) / 50);
	$xtick = int (($xmax-$xmin) / $numxtick);
	$ytick = int (($ymax-$ymin) / $numytick);
	$xtick = 1 unless ($xtick);
	$ytick = 1 unless ($ytick);
	$gmtcom =  "psxy /dev/null -JX$xaxlen/$yaxlen -R$xmin/$xmax/$ymin/$ymax ";
	$gmtcom .= "-B${xtick}:\"${xlabel}\":\/${ytick}:\"$ylabel\"::.\"$title\":WSen ";
	$gmtcom .= "-X$xoffset -Y$yoffset -P -K > $psfile";
	`$gmtcom`;

	# Plot symbols
	$gmtcom  = "psxy -JX$xaxlen/$yaxlen -R -S$symbol -O >> $psfile";
	open (GMTCOM,"| $gmtcom") || die "Cannot open GMT command\n";
	print GMTCOM $data;
	close (GMTCOM);
}	
`gs $psfile`;

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
