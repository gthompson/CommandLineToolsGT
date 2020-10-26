#!/usr/bin/perl -w
use strict;
use POSIX qw(log10);
use Time::Local;

# Name:       plot_cumul
# Source:     /data2/development/ampengfft/plot_cumul.pl
# Arguments:  see below
# Returns:    0 on success 1 on failure

# For plotting the result file from compile_slice
# This version of plotslice plots multiple cumulative energy lines on one graph

# Set default parameters, can be replaced from command line
my $title      = " ";                                      # default title of plot
my $infile     = "/home/seisan/WOR/compile_slice.dat";     # default file to be plotted
my $psfile     = "plot_cumul.ps";                          # name of resulting ps file
my $types      = "er";                                     # event type codes concattenated together
my $grid       = "";

# If plotting depth exclude depths that are very poorly known
my $max_deperr = 1;

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
						  
my @colours = ( "255/0/0","0,255,0","0/0/255","100/100/100","100/0/100","100/100/0");
	  
# Plot params
my $xoffset = 3.4;
my $yoffset = 4;
my $xaxlen = 17;
my $yaxlen = 23;
my $symbol = "x0.3";
my $xlabel = "";
my $ylabel = "";

# Script starts here
my ($usage);
my ($data,$gmtcom,$line,$line2);
my ($t1,$t2);
my ($num,$nsta);
my ($want_sta,@want,$num_want);
my (@sta,@stadat,$fac,@maxfac,@slice);
my (@datetime,@day,@day_label,$type);
my ($labely,$labelx);
my (@epoch,@eng);
my ($epoch_start,$xpar,$ypar,@x,@y);
my ($xmin,$xmax,$ymin,$ymax);
my ($numday,$time_interval,$day_label,$day_start);
my ($xtick,$xanot,$xanot2,$ytick);
my ($sec,$min,$hour,$mday,$mon,$year);
my ($yyyy,$mm,$dd,$hh,$mi);
my (@cumeng,$cumtot,$step);
my ($i,$j,$c);

$usage  = "USAGE: plot_cumul [in=filename] [t1=yyyymmddhh] [t2=yyyymmddhh] [title=] [types=] [grid=1] sta1 [sta2..]\n";

# Read command-line options, if any.  Defaults set above
$want_sta = ""; $num_want=0;
for ($i=0; $i<@ARGV; $i++){

	if  ($ARGV[$i] =~ /^in=(.+)$/){
		$infile = $1;
	}
	elsif ($ARGV[$i] =~ /^t1=(\d\d\d\d)(\d\d)(\d\d)(\d\d)?$/){
		if ($4){
			$t1="$1-$2-$3 $4:00:00";
		}
		else{
			$t1="$1-$2-$3 00:00:00";
		}
	}
	elsif ($ARGV[$i] =~ /^t2=(\d\d\d\d)(\d\d)(\d\d)(\d\d)?$/){
		if ($4){
			$t2="$1-$2-$3 $4:00:00";
		}
		else{
			$t2="$1-$2-$3 00:00:00";
		}
	}
	elsif ($ARGV[$i] =~ /^types=(\w+)$/){
		$types = $1;
	}
	elsif ($ARGV[$i] =~ /^([A-Z]+)$/){
		$want_sta .= "$1 ";
		$num_want++;
	}
	elsif ($ARGV[$i] =~ /title=(.+)/){
		$title = $1;
	}
	elsif ($ARGV[$i] =~ /grid=([01])/){
		$grid = "g1d" if ($1==1);
	}
	else {
		print STDERR "Unrecognised argument $i: $ARGV[$i].\n";
		die "$usage\n";
	}
}
unless ($want_sta){
	die "Missing argument $usage\n";
}
open (IN,"$infile") || die "Can't open $infile\n";

# Get station names from header
$line=<IN>; $i=0;
while ($line =~ s/(MB[A-Z]{2}) +//){
	$sta[$i]=$1;
	if ( $want_sta =~ /$sta[$i]/){
		$want[$i] = 1;
	}
	else{
		$want[$i] = 0;
	}		
	$i++;
}
unless ($i){
	die "Can't read header: $line\n";
}
$nsta=$i;

for ($i=0; $i<$nsta; $i++){
	$maxfac[$i]	 = 0;
}

$i=0; $epoch_start=10e20;
LINE:foreach $line (<IN>){
	unless ($line =~ s/^((.{10}).{9}) (\w)  (\d{10}) ([ \d]{7}) ([ \d]{3})//){
		print "line doesn't match pattern: $1\n";
		next;
	}
	if ($5 eq "       " or $6 eq "   "){
		print "line doesn't match pattern: $1\n" unless ($i==0);
		next;		
	}
	# Check date against possible command line options 
	next if ($t1 && $1 lt $t1);
	last if ($t2 && $1 gt $t2);
	$type = $3;
	
	# Seperate block to preserve $1 etc from pattern above
	{
		next LINE if ($types !~ /$type/);
	}

	$datetime[$i]=$1; $day[$i]=$2; $epoch[$i]=$4;
	$epoch_start = $epoch[$i] if ($epoch_start > $epoch[$i]);

	$j=0;
	while ($line =~ s/^(  (\d\.\d\de-(\d\d))( [ \d]{2})( [ \d]{2})( [ \d]{2})( [ \d]{2})( [ \d]{2})( [ \d]{2})( [ \d]{2})| {31})//){
		unless ($1 eq "                               "){
			$eng[$j][$i]=$2, $fac=$3;
			if ($eng[$j][$i] < $discard_if_less{$sta[$j]} ||
			    $eng[$j][$i] > $discard_if_more{$sta[$j]})
			{
				print "Discarding $eng[$j][$i] value for $sta[$j]\n";
				$eng[$j][$i]=0; $fac=0;
			}
			$maxfac[$j] = $fac if ($fac>$maxfac[$j]);
		}
		$j++;
	}
	if ($j < $nsta){
		print "Not enough params on line: $line\n";
		next;
	}
	$i++;
}
close(IN);
$num=$i;

die "No data found\n" if ($num==0);

($sec,$min,$hour,$mday,$mon,$year) = localtime($epoch_start);
$day_start = timelocal(0,0,0,$mday,$mon,$year);
for ($i=0; $i<$num; $i++){
	$x[$i] = ($epoch[$i] - $day_start)/86400;
}
$xlabel="Day";

# Find maxima for normalisation
for ($i=0; $i<$nsta; $i++){

	next unless ($want[$i]);
	
	$cumeng[$i] = 0;
	for ($j=0; $j<$num; $j++){
		next unless ($eng[$i][$j]);
		$cumeng[$i] += $eng[$i][$j] * "1e$maxfac[$i]";
	}
}

# Set GMT plotting defaults
$gmtcom  = "gmtset MEASURE_UNIT cm LABEL_FONT_SIZE 18 HEADER_FONT_SIZE 24 ";
$gmtcom .= "ANOT_FONT_SIZE 12 GRID_PEN_PRIMARY 0.01c";
`$gmtcom`;

# Plot axes
$xmin = $x[0];
$xmax = $x[$num-1];
$ymin = 0;
$ymax = 100 + $num_want*10 + 1;
$ytick = 0;
$ylabel= "Normalised Cumulative Energy";

# Week annotation stops overlap at month ends but only works with tick of 1 or 7
# If more than a few months don't want days at all
if (($epoch[$num-1] - $epoch[0])/86400 < 31){
	$xtick  = 1;
	$xanot  = 1;
	$xanot2 = 1;
}
elsif (($epoch[$num-1] - $epoch[0])/86400 < 180){
	$xtick  = 1;
	$xanot  = 7;
	$xanot2 = 1;
}
else{
	$xtick  = 0;
	$xanot  = 0;
	$xanot2 = 3;
}
`gmtset TIME_FORMAT_SECONDARY abbreviated`;
`gmtset PLOT_DATE_FORMAT o-yyyy`;
	
$gmtcom =  "psxy /dev/null -JX${xaxlen}t/$yaxlen -f0T -R$xmin/$xmax/$ymin/$ymax ";
$gmtcom .= "-Bpa${xanot}Rf${xtick}d${grid}\/${ytick}:\"$ylabel\"::.\"$title\":WSen -Bs${xanot2}O\/WSen ";
$gmtcom .= "-X$xoffset -Y$yoffset -P -K > $psfile";
`$gmtcom`;
	
$step=0;
for ($i=0; $i<$nsta; $i++){

	next unless ($want[$i]);
	
	$data="";
	$cumtot = 0;
	for ($j=0; $j<$num; $j++){
		next unless ($eng[$i][$j]);
		$cumtot += 100* ($eng[$i][$j] * "1e$maxfac[$i]")/$cumeng[$i];
		$data .= "$x[$j] " . ($cumtot+$step*10) . "\n";
	}
	$step++;

	# Plot line
	$c = pop @colours;
	$gmtcom  = "psxy -JX$xaxlen/$yaxlen -R -O -K -W1/$c >> $psfile";
	open (GMTCOM,"| $gmtcom") || die "Cannot open GMT command\n";
	print GMTCOM $data;
	close (GMTCOM);

	# line for key
	$gmtcom  = "psxy -JX$xaxlen/$yaxlen -R -O -K -W1/$c <<END>> $psfile\n";
	$labely = $ymax - 5*$step;;
	$labelx = $xmin + 1*($xmax-$xmin)/20;
	$gmtcom .= "$labelx $labely\n";
	$labelx = $xmin + 2*($xmax-$xmin)/20;
	$gmtcom .= "$labelx $labely\n";
	$gmtcom .= "END\n";
	`$gmtcom`;

	# Add label
	$gmtcom  = "pstext -JX$xaxlen/$yaxlen -R$xmin/$xmax/$ymin/$ymax -N -O -K <<END>> $psfile\n";
	$labelx = $xmin + 2.1*($xmax-$xmin)/20;
	$gmtcom .= "$labelx $labely 18 0 0 ML $sta[$i]\n";
	$gmtcom .= "END\n";
	`$gmtcom`;
}

`gs $psfile`;
exit(0);


