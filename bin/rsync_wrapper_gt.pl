#!/usr/bin/perl -w
use strict;
use File::Path;
use File::Spec qw(catpath);
#use GetOpt::Long::Descriptive;
my @options=@ARGV;
my $usage = "$0: pfname\nRun this with a parameter file with name-value pairs that specifies a TARGET and one or many SOURCE directories.\n";
$usage = $usage."For example:\nTARGET /media/prettygood/computers/hal9000\nSOURCE /media/sda1\n";
die($usage) unless($#ARGV==0);

#our $RSYNC_COMMAND = "rsync -avcm --progress --stats";
my $SAFEMODE = 0;
our $RSYNC_COMMAND = "rsync -av --delete --dry-run";
unless ($SAFEMODE) {
	$RSYNC_COMMAND = "rsync -av --delete";
}

# Read parameter file
my @hostdirlist;
our $TARGET_TOP;
my $filename = $ARGV[0];
die("No such parameter file $filename") unless (-f $filename);
open(FH, '<', $filename) or die $!;
while(<FH>){
   my @fields = split /\s+/, $_;
   if ($fields[0] eq "TARGET") {
	$TARGET_TOP = $fields[1];
   }
   if ($fields[0] eq "SOURCE") {
	push @hostdirlist, $fields[1];
   }
   
}
close(FH);
die("Bad parameter file\n$usage") unless ($TARGET_TOP && $#hostdirlist > -1);

my $hostdir;
foreach $hostdir (@hostdirlist) {
	my @all_dirs = glob("$hostdir/[a-z|A-Z|0-9]*"); # only backup directories that start with a letter or number
	foreach my $thisdir (@all_dirs) {
		if (-d $thisdir) {
			&rsync_this_directory($thisdir);
		} elsif (-f $thisdir) {
			my $thisfile = $thisdir;
			&rsync_this_file($thisfile);
		}
	}
}
1;

sub check_mountpoint {
	# Check mountpoint is mounted - if not, try to mount, then fail
	# Assumes all mountpoints are two levels deep, e.g. /raid/data, /raid/home, /media/prettygood, /media/sda1
	my ($dir) = @_;
	my @dummy = split("/", $dir);
	my $mountpoint = "/$dummy[1]/".$dummy[2];
	$mountpoint = "/home" if $dummy[1] eq "home"; # exception for /home, not 2 levels
	
	my $result = `mountpoint $mountpoint`;
	chomp($result);
	if ($result eq "$mountpoint is not a mountpoint") {
		# Try to mount?
		system("mount -a");
		my $result2 = `mountpoint $mountpoint`;
		chomp($result2);
		if ($result2 eq "$mountpoint is not a mountpoint") {
			print("- $dir: $mountpoint not mounted\n");
			return 0;
		}
	}
	print("- $dir: $mountpoint is mounted\n");
	return 1;
}

sub rsync_this_file {
	my $sourcefile = $_[0];
	our $TARGET_TOP;
	my $targetdir = $TARGET_TOP.$sourcefile;
	return unless (&check_mountpoint($sourcefile));
	return unless (&check_mountpoint($TARGET_TOP));

	if (-d $targetdir) { # Ensure the target file is not a directory. If blank direcory, remove it.
		my $numtargetfiles = `find $targetdir  -type f 2> /dev/null | wc -l`;
		chomp($numtargetfiles);
		if ($numtargetfiles == 0)  {
			rmpath($targetdir) unless $SAFEMODE;
		} else {
			print "- ERROR Source file exists as directory on target, and $targetdir is not empty\n";
		}
	} else {
		my $targetfile = $targetdir;
		my $parentdir = dirname $targetfile;
		unless (-d $parentdir) {
			print "- Need to mkpath $parentdir\n";
			unless ($SAFEMODE or mkpath($parentdir)) {
				print "- ERROR Could not mkpath $parentdir\n";
				my $end_ts = &getTimeStamp();
				print "$RSYNC_COMMAND, $sourcedir/, $targetdir, # source files, $numsourcefiles, # target files, -999, mkpath failed, $start_ts, $end_ts\n";
				return;
			}
		}
		my $start_ts = &getTimeStamp();
		print "- Starting rsync $sourcefile to $targetfile at $start_ts. ";
		my $result = `$RSYNC_COMMAND $sourcefile $targetfile`;
		my $end_ts = &getTimeStamp();
		print "- Ending at $end_ts\n";
		print "$RSYNC_COMMAND, $sourcefile, $targetfile, # source files, 1, # target files, 1, target not writeable, $start_ts, $end_ts\n";
	}
	return;
}

sub rsync_this_directory {

	my ($input_dir) = @_;
	our $TARGET_TOP;

	my $total_start_ts = &getTimeStamp();
	print "\n***\nStarting syncing $input_dir to $TARGET_TOP at $total_start_ts\n";
	return unless (&check_mountpoint($input_dir));
	return unless (&check_mountpoint($TARGET_TOP));

	unless (-d $input_dir) { 
		# Can't find the source, so quit this subroutine
		print "- ERROR Source Directory $input_dir not found\n";
		return;
	} else {
		print "- $input_dir found\n";
	}

	my @alldirs = glob("$input_dir/*");
	foreach my $sourcedir (@alldirs) {
		print("\nProcessing $sourcedir\n");
		
		# check sourcedir really is a dir - if it is just a file, then just copy
		if (-d $sourcedir) {
			print "- Processing $sourcedir\n";
			my $start_ts = &getTimeStamp();

			my $targetdir = "$TARGET_TOP/$sourcedir";
			my $cmd = "$RSYNC_COMMAND $sourcedir/ $targetdir";
			my $numsourcefiles = `find $sourcedir  -type f 2> /dev/null | wc -l`;
			chomp($numsourcefiles);

			unless (-d $targetdir) {
				print "- Need to mkpath $targetdir\n";
				unless ($SAFEMODE or mkpath($targetdir)) {
					print "- ERROR Could not mkpath $targetdir\n";
					my $end_ts = &getTimeStamp();
					print "$RSYNC_COMMAND, $sourcedir/, $targetdir, # source files, $numsourcefiles, # target files, -999, mkpath failed, $start_ts, $end_ts\n";
					return;
				}
			} else {
				print "- $targetdir already exists.\n";
				# UNCOMMENT FOLLOWING 2 LINES TO SPEED UP FIRST COPY OF EVERYTHING (BUT IT MIGHT MISS SOME THINGS)
				#print "Assuming it has already been backed up. Jumping to next sourcedir\n";
				#return;
			}
			my $numtargetfiles = `find $targetdir  -type f 2> /dev/null | wc -l`;
			chomp($numtargetfiles);

			if (-d $targetdir and -w $targetdir) {
				print "- Starting rsync $sourcedir to $targetdir at $start_ts. ";
				my $result = `$cmd`;
				my $numtargetfilesold = $numtargetfiles;
				$numtargetfiles = `find $targetdir  -type f 2> /dev/null | wc -l`;
				chomp($numtargetfiles);
				my $numnewfiles = $numtargetfiles - $numtargetfilesold;
				my $end_ts = &getTimeStamp();
				print "- Ending at $end_ts\n";
				print "$RSYNC_COMMAND, $sourcedir/, $targetdir, # source files, $numsourcefiles, # target files, $numtargetfiles, added $numnewfiles, $start_ts, $end_ts\n";
			} else {
				print "$RSYNC_COMMAND, $sourcedir/, $targetdir, # source files, $numsourcefiles, # target files, $numtargetfiles, target not writeable, $start_ts, -996\n";
			}
		} elsif (-f $sourcedir) {
			&rsync_this_file($sourcedir);

		} else {
			print "$RSYNC_COMMAND, $sourcedir/, what is source?,  # source files, -998, # target files, -998, weirdness happened, -997, -997\n";
		}	

	}

	my $total_end_ts = &getTimeStamp();
	print "Finished syncing $input_dir to $TARGET_TOP at $total_end_ts\n\*\n";


}

sub getTimeStamp {
	my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);
	my $ts = sprintf("%04d/%02d/%02d %02d:%02d:%02d", $year+1900, $mon+1, $mday,  $hour, $min, $sec);
	return $ts;
}
