#!/usr/bin/perl

use CGI qw(:standard);
use CGI::Carp qw(fatalsToBrowser);
use JSON;
use Time::HiRes qw(usleep);


print header('application/json');

my $i = 0;
while(-e "../../Control/ramdisk/weblog.txt_LOCK") { if($i > 1000) { last; } usleep(1000); $i++; }

open (LOGFILE, "../../Control/ramdisk/weblog.txt");
my @LogLines = <LOGFILE>;
close LOGFILE;

my $PlotInfo;

my $i = 0;

foreach my $LogLine (@LogLines) {
	
	my ($LineType, $LineContents) = split(/\|/,$LogLine);

	$LineContents =~ s/\n//;

	if($LineType eq 'Log') { $PlotInfo->{"log"} .= $LineContents . "\n"; }
	else {

		if($LineType eq 'GPS Latitude') { $PlotInfo->{"gps"}->{"lat"} = $LineContents; }
		elsif($LineType eq 'GPS Longitude') { $PlotInfo->{"gps"}->{"long"} = $LineContents; }
		$PlotInfo->{"params"}->{$LineType}->{"content"} = $LineContents;
		$PlotInfo->{"params"}->{$LineType}->{"order"} = $i;
		$i++;
	}

}

$i = 0;
while(-e "../../Control/ramdisk/runningmap.txt_LOCK") { if($i > 1000) { last; } usleep(1000); $i++; }

open (MAPFILE, "../../Control/ramdisk/runningmap.txt");
my @MapLines = <MAPFILE>;
close MAPFILE;

push(@{$PlotInfo->{"mapdata"}},[$PlotInfo->{"params"}->{"Fused X Pos"}->{"content"}*1.0, $PlotInfo->{"params"}->{"Fused Y Pos"}->{"content"}*1.0, "blue"]);

foreach my $MapLine (@MapLines) {

	$MapLine =~ s/\n//;
	my ($LineType, @LineContents) = split(/,/,$MapLine);

	my $Colour = 'black';

	if($LineType eq 'D') { next; }
	elsif($LineType =~ /^[0-9]+$/) { 
		if($LineType == $PlotInfo->{"params"}->{"NextWaypoint"}->{"content"} && $PlotInfo->{"params"}->{"Advanced Path Planning Active"}->{"content"} == 0) { $PlotInfo->{"nextwp"}->{"x"} = $LineContents[0]*1.0;  $PlotInfo->{"nextwp"}->{"y"} = $LineContents[1]*1.0; } 
		if($LineType < $PlotInfo->{"params"}->{"NextWaypoint"}->{"content"} && $PlotInfo->{"params"}->{"Advanced Path Planning Active"}->{"content"} == 0) { $Colour = "yellowgreen"; }
		else { $Colour = "green"; }
	}
	elsif($LineType eq 'F') { $Colour = 'orange'; }
	elsif($LineType eq 'I') { $Colour = 'brown'; }
	
	if($PlotInfo->{"params"}->{"Advanced Path Planning Active"}->{"content"} == 1) {
		if($LineType eq 'S') { 
			if($LineContents[0] == $PlotInfo->{"params"}->{"NextWaypoint"}->{"content"}) { $PlotInfo->{"nextwp"}->{"x"} = $LineContents[1]*1.0;  $PlotInfo->{"nextwp"}->{"y"} = $LineContents[2]*1.0; } 
			$Colour = 'green';
		}
		if($LineType eq 'P') { 
			$Colour = 'blue';
		}
		if ($LineType eq 'S' || $LineType eq 'P') {
			push(@{$PlotInfo->{"mapdata"}},[$LineContents[1]*1.0, $LineContents[2]*1.0, $Colour]);
		} else {
			push(@{$PlotInfo->{"mapdata"}},[$LineContents[0]*1.0, $LineContents[1]*1.0, $Colour]);
		} 
	} else {
		push(@{$PlotInfo->{"mapdata"}},[$LineContents[0]*1.0, $LineContents[1]*1.0, $Colour]);
	}
	
	

	

}

my $json = JSON->new->allow_nonref;

print $json->encode($PlotInfo);

