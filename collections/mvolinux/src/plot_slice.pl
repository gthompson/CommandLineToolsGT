#!/usr/bin/perl -w
use strict;
use POSIX qw(log10);
use Time::Local;

# Name:           plot_slice
# Source:         /data2/development/ampengfft/plot_slice.pl
# Arguments:      see below
# Returns:        0 on success 1 on failure

# For plotting the result file from compile_slice
# Select station with first number after parameter eg eng1 is eng on MBGH
# Select second station for ratio with rat second number 
#	eg rat12 is eng MBGH/ eng MBLG
# Select slices for frequency split using slice numbers 2 and 3 
#   eg slice111 is MBGH energy between 0 and 1

# Set default parameters, can be replaced from command line
my $title  = " ";                                  # default title of plot
my $infile = "/home/seisan/WOR/compile_slice.dat"; # default file to be plotted
my $psfile = "plot_slice.ps";                      # name of resulting ps file
my $types  = "er";                                 # event type codes concattenated together
my $num_hour_bin = 24;                             # default to day bin for bineng

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
                          "MBLG" => 1.00e-4,
                          "MBRY" => 1.00e-6,
                          "MBFR" => 1.00e-6,
                          "MBBY" => 1.00e-6,
                          "MBGB" => 1.00e-6  );						  
# Plot params
my $xoffset = 3.4;
my $yoffset = 4;
my $xaxlen = 17;
my $yaxlen = 23;
my $symbol = "-Sx0.3";            # overridden by some choices of ypar
my $shade = "100/200/100";        # nasty greeny thing
my $xlabel = "";
my $ylabel = "";
my $numxtick = 15;
my $numytick = 15;

# Script starts here
my ($usage);
my ($data,$gmtcom,$line,$line2);
my ($t1,$t2);
my ($num,$xsta,$ysta,$nsta);
my ($xmax_clip,$ymax_clip,$outfile);
my ($xrat,$yrat,$xslice1,$xslice2,$yslice1,$yslice2);
my ($dep,$deperr);
my (@sta,@stadat,$fac,@maxfac,@slice);
my (@datetime,@day,@day_label,$type);
my ($labely,$labelx);
my (@epoch,@repose,@duration,@eng,@dep);
my ($epoch_start,$xpar,$ypar,@x,@y);
my ($xmin,$xmax,$ymin,$ymax);
my ($numday,$time_interval,$day_label,$day_start);
my ($xmaxtime,$xmintime,$ymaxtime,$ymintime);
my ($xtick,$xanot,$xanot2,$ytick,$bin);
my ($sec,$min,$hour,$mday,$mon,$year,@hourbin);
my ($yyyy,$mm,$dd,$hh,$mi);
my ($pattern,@sfiles);
my ($cumtot);
my ($i,$j);

$usage  = "USAGE: plot_slice [in=filename] [t1=yyyymmddhh] [t2=yyyymmddhh]\n";
$usage .= "  [title=] [types=] [xmax=] [ymax=] [out=filename]\n";
$usage .= "  x=[epoch|rep|dur|eng\\d|logeng\\d|slice\\d\\d\\d|rat\\d\\d|dep]\n";
$usage .= "  y=[num|epoch|rep|dur|eng\\d|logeng\\d||cumeng\\d|bineng\\d[\\d+]|slice\\d\\d\\d|rat\\d\\d|dep]\n";

# Read command-line options, if any.  Defaults set above
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
	elsif ($ARGV[$i] =~ /x=(epoch|rep|dur|eng\d|logeng\d|slice\d\d\d|rat\d\d|dep)$/){
		$xpar = $1;
		if ($xpar =~ /^(logeng|eng)(\d)/){
			$xpar = $1;
			$xsta = $2 - 1;
			die "Bad x argument $ARGV[$i]\n" if $xsta<0;
		}
		elsif ($xpar =~ /^rat(\d)(\d)/){
			$xpar = "rat";
			$xsta = $1 - 1;
			$xrat = $2 - 1;
			die "Bad x argument $ARGV[$i]\n" if ($xsta<0);
			die "Bad x argument $ARGV[$i]\n" if ($xrat<0);
		}
		elsif ($xpar =~ /^slice(\d)(\d)(\d)/){
			$xpar = "slice";
			$xsta = $1 - 1;
			$xslice1 = $2;
			$xslice2 = $3;
			die "Bad x argument $ARGV[$i]\n" if ($xsta<0);
			die "Bad x argument $ARGV[$i]\n" if ($xslice1<1 or $xslice1>7);
			die "Bad x argument $ARGV[$i]\n" if ($xslice2<1 or $xslice2>7);
		}
		elsif ($xpar !~ /(epoch|rep|dur|dep)/){
			die "Unrecognised x argument $xpar\n";
		}
	}
	elsif ($ARGV[$i] =~ /y=(num|epoch|rep|dur|eng\d|logeng\d|cumeng\d|bineng\d+|slice\d\d\d|rat\d\d|dep)$/){
		$ypar = $1;
		if ($ypar =~ /^(logeng|eng|cumeng)(\d)/){
			$ypar = $1;
			$ysta = $2 - 1;
			die "Bad y argument $ARGV[$i]\n" if $ysta<0;
		}
		elsif ($ypar =~ /^(bineng)(\d)(\d+)?/){
			$ypar = $1;
			$ysta = $2 - 1;
			$num_hour_bin = $3 if ($3);
			die "Bad y argument $ARGV[$i]\n" if $ysta<0;
		}
		elsif ($ypar =~ /^rat(\d)(\d)/){
			$ypar = "rat";
			$ysta = $1 - 1;
			$yrat = $2 - 1;
			die "Bad y argument $ARGV[$i]\n" if ($ysta<0);
			die "Bad y argument $ARGV[$i]\n" if ($yrat<0);
		}
		elsif ($ypar =~ /^slice(\d)(\d)(\d)/){
			$ypar = "slice";
			$ysta = $1 - 1;
			$yslice1 = $2;
			$yslice2 = $3;
			die "Bad y argument $ARGV[$i]\n" if ($ysta<0);
			die "Bad y argument $ARGV[$i]\n" if ($yslice1<0 or $yslice1>5);
			die "Bad y argument $ARGV[$i]\n" if ($yslice2<0 or $yslice2>5);
		}
		elsif ($ypar !~ /(num|epoch|rep|dur|dep)/){
			die "Unrecognised y argument $ypar\n";
		}
	}
	elsif ($ARGV[$i] =~ /xmax=([\d\.]+)/){
		$xmax_clip = $1;
	}
	elsif ($ARGV[$i] =~ /ymax=([\d\.]+)/){
		$ymax_clip = $1;
	}
	elsif ($ARGV[$i] =~ /title=(.+)/){
		$title = $1;
	}
	elsif ($ARGV[$i] =~ /out=(.+)/){
		$outfile = $1;
	}
	else {
		print STDERR "Unrecognised argument $i: $ARGV[$i].\n";
		die "$usage\n";
	}
}

# Open input file
open (IN,"$infile") || die "Can't open $infile\n";

# Get station names from header
$line=<IN>; $i=0;
while ($line =~ s/(MB[A-Z]{2}) +//){
	$sta[$i++]=$1
}
unless ($i){
	die "Can't read header: $line\n";
}
$nsta=$i;

# Want numbers for stations in usage message
unless ($xpar && $ypar){
	printf STDERR "Missing argument\n$usage";
	for ($i=0;$i<$nsta;$i++){
		printf STDERR "$sta[$i] = " . ($i+1) . "\n";
	}
	die "\n";
}
if ($ypar eq "cumeng" && $xpar ne "epoch"){
	die "Cumulative energy only makes sense plotted against epoch\n";
}
if ($ypar eq "bineng" && $xpar ne "epoch"){
	die "Daily energy only makes sense plotted against epoch\n";
}
if ($ypar eq "num" && $xpar eq "epoch"){
	die "Binning days for a histogram makes no sense\n";
}

if (($xsta && $xsta > $nsta) || ($ysta && $ysta > $nsta)){
	die "Not enough stations\n";
}

for ($i=0; $i<$nsta; $i++){
	$maxfac[$i]	 = 0;
}
 
$i=0; $epoch_start=10e20;
LINE:foreach $line (<IN>){
	unless ($line =~ s/^((.{10}).{9}) (\w)  ([ \d]{10}) ([ \d]{7}) ([ \d]{3})//){
		print "line doesn't match pattern: $line\n";
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

	$datetime[$i]=$1; $day[$i]=$2; $epoch[$i]=$4*1; $repose[$i]=$5*1; $duration[$i]=$6*1;
	$epoch_start = $epoch[$i] if ($epoch_start > $epoch[$i]);

	# Want epoch start to nearest hour
	($sec,$min,$hour,$mday,$mon,$year) = localtime($epoch_start);
	$epoch_start = timelocal(0,0,$hour,$mday,$mon,$year);


	# To plot depth need to find s-file
	if ($xpar eq "dep" or $ypar eq "dep"){
		$datetime[$i] =~ /(\d\d\d\d)\-(\d\d)\-(\d\d) (\d\d):(\d\d)/;
		$yyyy=$1; $mm=$2; $dd=$3; $hh=$4; $mi=$5;

		$pattern = "$ENV{EVENT_PATH}/$yyyy/$mm/$dd-$hh$mi*";
		@sfiles = `ls $pattern 2>/dev/null`;
		
		if (@sfiles == 0){
			$mi--;
			$pattern = "$ENV{EVENT_PATH}/$yyyy/$mm/$dd-$hh$mi*";
			@sfiles = `ls $pattern 2>/dev/null`;
		}
		if (@sfiles != 1){
			print "Too few or too many s-files found $datetime[$i]\n";
			next LINE;
		}
		$line2 = `head -1 $sfiles[0]`;
		next LINE unless ($line2 =~ /-62.\d\d\d +(\d+\.\d)/);
		$dep =$1;

		# Check depth error
		$line2 = `grep GAP $sfiles[0]`;
		next LINE unless ($line2 =~ /GAP=[\d\. ]{33} +(\d+\.\d)/);		
		$deperr =$1;
		
		next LINE if ($deperr > $max_deperr);
		$dep[$i] =$dep;
	}

	$j=0;
	while ($line =~ s/^(  (\d\.\d\de-(\d\d))( [ \d]{2})( [ \d]{2})( [ \d]{2})( [ \d]{2})( [ \d]{2})( [ \d]{2})( [ \d]{2})| {31})//){
		unless ($1 eq "                               "){
			$eng[$j][$i]=$2, $fac=$3;
			$slice[$j][$i][1]=$4*1;
			$slice[$j][$i][2]=$5*1;
			$slice[$j][$i][3]=$6*1;
			$slice[$j][$i][4]=$7*1;
			$slice[$j][$i][5]=$8*1;
			$slice[$j][$i][6]=$9*1;
			$slice[$j][$i][7]=$10*1;
			if ($eng[$j][$i] < $discard_if_less{$sta[$j]} ||
			    $eng[$j][$i] > $discard_if_more{$sta[$j]})
			{
				if (($xpar =~ /eng/ and $xsta==$j) or ($ypar =~ /eng/ and $ysta==$j)){
					print "Discarding $eng[$j][$i] value for $sta[$j]\n";
				}
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

if ($xpar eq "epoch"){
	for ($i=0; $i<$num; $i++){
		$x[$i] = substr($datetime[$i],0,10) . "T" . substr($datetime[$i],11);
	}
}
elsif ($xpar eq "rep"){
	@x = @repose;
    $xlabel="Repose Time (sec)";
}
elsif ($xpar eq "dep"){
	@x = @dep;
    $xlabel="Depth (km)";
}
elsif ($xpar eq "dur"){
	@x = @duration;
	$xlabel="Duration (sec)";
}
elsif ($xpar eq "logeng"){
	$j=0;
	for ($i=0; $i<$num; $i++){
		next unless ($eng[$xsta][$i]);
		$x[$i] = POSIX::log10($eng[$xsta][$i]);
		$j++;
	}
	die "No data for station $sta[$xsta]\n" unless ($j);
	$xlabel="Log energy on $sta[$xsta] (types=$types)";
}
elsif ($xpar eq "eng"){
	$j=0;
	for ($i=0; $i<$num; $i++){
		next unless ($eng[$xsta][$i]);
		$x[$i] = $eng[$xsta][$i] * "1e$maxfac[$xsta]";
		$j++;
	}
	die "No data for station $sta[$xsta]\n" unless ($j);
	$xlabel="Energy on $sta[$xsta] (e-$maxfac[$xsta]) J/kg (types=$types)";
}
elsif ($xpar =~ /slice/){
	for ($i=0; $i<$num; $i++){
		$x[$i] = 0;
		for ($j=$xslice1; $j<=$xslice2; $j++){
			if ($slice[$xsta][$i][$j]){
				$x[$i] += $slice[$xsta][$i][$j];
			}
		}
	}
	$xlabel  = "% Energy between ";
	$xlabel .= $xslice1-1;
	$xlabel .= " and $xslice2 Hz on $sta[$xsta]  (types=$types)";
}
elsif ($xpar =~ /rat/){
	$j=0;
	for ($i=0; $i<$num; $i++){
		next unless ($eng[$xsta][$i] && $eng[$xrat][$i]);
		$x[$i] = $eng[$xsta][$i]/$eng[$xrat][$i];
		$j++;
	}
	die "No data for station $sta[$ysta]\n" unless ($j);
	$xlabel="Energy $sta[$xsta] / Energy $sta[$xrat] (types=$types)";
}

if ($ypar eq "epoch"){
	for ($i=0; $i<$num; $i++){
		$y[$i] = ($epoch[$i] - $epoch_start)/86400;
	}
    $ylabel="Days since $day[0]";
}
elsif ($ypar eq "rep"){
	@y = @repose;
    $ylabel="Repose Time (sec)";
}
elsif ($ypar eq "dep"){
	@y = @dep;
    $ylabel="Depth (km)";
}
elsif ($ypar eq "dur"){
	@y = @duration;
	$ylabel="Duration (sec)";
}
elsif ($ypar eq "logeng"){
	$j=0;
	for ($i=0; $i<$num; $i++){
		next unless ($eng[$ysta][$i]);
		$y[$i] = POSIX::log10($eng[$ysta][$i]);
		$j++;
	}
	die "No data for station $sta[$ysta]\n" unless ($j);
	$ylabel="Log10 energy on $sta[$ysta] (types=$types)";
}
elsif ($ypar eq "eng"){
	$j=0;
	for ($i=0; $i<$num; $i++){
		next unless ($eng[$ysta][$i]);
		$y[$i] = $eng[$ysta][$i] * "1e$maxfac[$ysta]";
		$j++;
	}
	die "No data for station $sta[$ysta]\n" unless ($j);
	$ylabel="Energy on $sta[$ysta] (e-$maxfac[$ysta]) J/kg (types=$types)";
}
elsif ($ypar eq "cumeng"){
	$cumtot = 0;
	for ($i=1; $i<$num; $i++){
		next unless ($eng[$ysta][$i]);
		$cumtot += $eng[$ysta][$i] * "1e$maxfac[$ysta]";
		$y[$i] = $cumtot;
	}
	die "No data for station $sta[$ysta]\n" unless ($cumtot);
	
	if ($types eq "er"){
		$ylabel="Cumulative Rockfall Energy on $sta[$ysta] (e-$maxfac[$ysta]) J/kg";
	}
	else{
		$ylabel="Cumulative Energy on $sta[$ysta] (e-$maxfac[$ysta]) J/kg (types=$types)";
	}
	# If y parameter is cumeng then want a line 
	$symbol = "";

}
elsif ($ypar =~ /slice/){
	for ($i=0; $i<$num; $i++){
		$y[$i] = 0;
		for ($j=$yslice1; $j<=$yslice2; $j++){
			if ($slice[$ysta][$i][$j]){
				$y[$i] += $slice[$ysta][$i][$j];
			}
		}
	}
	$ylabel  = "% Energy between ";
	$ylabel .= $yslice1-1;
	$ylabel .= " and $yslice2 Hz on $sta[$ysta]";
}
elsif ($ypar =~ /rat/){
	$j=0;
	for ($i=0; $i<$num; $i++){
		next unless ($eng[$ysta][$i] && $eng[$yrat][$i]);
		$y[$i] = $eng[$ysta][$i]/$eng[$yrat][$i];
		$j++;
	}
	die "No data for station $sta[$ysta]\n" unless ($j);
	$ylabel="Energy $sta[$ysta] / Energy $sta[$yrat] (types=$types) ";
}
elsif ($ypar eq "bineng"){

	# Bin by hour first 
	for ($i=0; $i<$num; $i++){
		next unless ($eng[$ysta][$i]);
		$j = int(($epoch[$i] - $epoch_start)/3600);
		$hourbin[$j] += $eng[$ysta][$i] * "1e$maxfac[$ysta]";
	}
	$num = $j+1;

	# Then combine hours into bigger bins if required
	for ($i=0; $i<$num; $i++){
		if ($i%$num_hour_bin == 0){
			$j=$i/$num_hour_bin;
			$epoch[$j] = $i*3600;
			($sec,$min,$hour,$mday,$mon,$year) = localtime($epoch_start+$epoch[$j]);

			$x[$j]        = sprintf("%d-%02d-%02dT%02d:00:00",$year+1900,$mon+1,$mday,$hour);
			$datetime[$j] = sprintf("%d-%02d-%02d %02d:00:00",$year+1900,$mon+1,$mday,$hour);
		}
		next unless ($hourbin[$i]);
		$y[$j] += $hourbin[$i];
	}
	die "No data for station $sta[$ysta]\n" unless ($num);
	$num = $j+1;

	$ylabel="Energy on $sta[$ysta] (e-$maxfac[$ysta]) J/kg  bin=$num_hour_bin hours (types=$types)";
	
	# If y parameter is bineng then want a bar chart 
	$symbol = "-Sb" . ($num_hour_bin/48);
}

# Set GMT plotting defaults
$gmtcom  = "gmtset MEASURE_UNIT cm LABEL_FONT_SIZE 18 HEADER_FONT_SIZE 24 ";
$gmtcom .= "ANOT_FONT_SIZE 12 GRID_CROSS_SIZE 0.15";
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
	$xmax = $xmax_clip if (defined $xmax_clip and $xmax > $xmax_clip);
	$bin = int(($xmax-$xmin)/20);
	$bin = int(($xmax-$xmin)/2)/10 if ($bin==0);
	print "fac=$maxfac[$xsta] " if ($xpar eq "eng");
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
# If want time on x-axis use GMT4 for better labels
elsif ($xpar eq "epoch"){

	# Somtimes want to write csv files
	if ($outfile){
		open (OUT,">$outfile") or die "Can't open $outfile\n";
		print OUT "date, $ylabel\n";
	}
	$data="";
	$ymax=-10e20; $ymin=10e20;
	for ($i=0; $i<$num; $i++){
		next unless ($x[$i]);
		next unless ($y[$i]);
		$data .= "$x[$i] $y[$i]\n";
		$ymin = $y[$i], $ymintime=$datetime[$i] if ($y[$i]<$ymin);	
		$ymax = $y[$i], $ymaxtime=$datetime[$i] if ($y[$i]>$ymax);
		print OUT "$datetime[$i], $y[$i]\n" if ($outfile);
	}
	close(OUT) if ($outfile);

	# Useful information to terminal
	print "\n$datetime[0] to $datetime[$num-1] num=$num\n";
	print "$sta[$ysta] types = $types\n";
	printf ("min y $ymintime %8.2f ",$ymin);
	print "* 1e$maxfac[$ysta]" if ($ypar =~ /^(cum|bin)?eng/ );
	printf ("\nmax y $ymaxtime %8.2f ",$ymax);
	print "* 1e$maxfac[$ysta]" if ($ypar =~ /^(cum|bin)?eng/ );
	print "\n";
	
	# Plot axes
	$xmin = $x[0];
	$xmax = $x[$num-1];
	$ymin -= int (1 + ($ymax-$ymin) / 50);
	$ymax = $ymax_clip if (defined $ymax_clip and $ymax > $ymax_clip);
	$ymax += int (1 + ($ymax-$ymin) / 50);
	$ytick = int (($ymax-$ymin) / $numytick);
	$ytick = 1 unless ($ytick);
	
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
	$gmtcom .= "-Bpa${xanot}Rf${xtick}dg${xtick}d\/${ytick}:\"$ylabel\"::.\"$title\":WS ";
	$gmtcom .= "-Bs${xanot2}O\/WSen ";
	$gmtcom .= "-X$xoffset -Y$yoffset -P -K > $psfile";
	`$gmtcom`;

	# Plot symbols
	$gmtcom  = "psxy -J -f0T -R $symbol -O >> $psfile";
	open (GMTCOM,"| $gmtcom") || die "Cannot open GMT command\n";
	print GMTCOM $data;
	close (GMTCOM);

}
# Otherwise want a scatter plot
else{
	# Somtimes want to write csv files
	if ($outfile){
		open (OUT,">$outfile") or die "Can't open $outfile\n";
		print OUT "date, $ylabel\n";
	}
	
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
		print OUT "$datetime[$i], $y[$i]\n" if ($outfile);
	}
	$xmax = $xmax_clip if (defined $xmax_clip and $xmax > $xmax_clip);
	$ymax = $ymax_clip if (defined $ymax_clip and $ymax > $ymax_clip);
	close(OUT) if ($outfile);
	print "xfac=$maxfac[$xsta] " if ($xpar eq "eng");
	print "yfac=$maxfac[$ysta] " if ($ypar eq "eng");
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
	$gmtcom =  "psxy /dev/null -JX${xaxlen}/$yaxlen -R$xmin/$xmax/$ymin/$ymax ";
	$gmtcom .= "-B${xtick}:\"${xlabel}\":\/${ytick}:\"$ylabel\"::.\"$title\":WSen ";
	$gmtcom .= "-X$xoffset -Y$yoffset -P -K > $psfile";
	`$gmtcom`;

	# Plot symbols
	$gmtcom  = "psxy -JX${xaxlen}/$yaxlen -R $symbol -O >> $psfile";
	open (GMTCOM,"| $gmtcom") || die "Cannot open GMT command\n";
	print GMTCOM $data;
	close (GMTCOM);
}	

# Reset GMT defaults
`gmtdefaults -Ds > .gmtdefaults4`;

# Plot picture
`gs $psfile`;

exit(0);


