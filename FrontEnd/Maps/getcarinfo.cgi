#!/usr/bin/perl

use CGI qw(:standard);
use CGI::Carp qw(fatalsToBrowser);
use JSON;
use Time::HiRes qw(usleep);


print header('application/json');

my $i = 0;
while(-e "../../TestCode/Control/ramdisk/weblog.txt_LOCK") { if($i > 1000) { last; } usleep(1000); $i++; }

open (LOGFILE, "../../TestCode/Control/ramdisk/weblog.txt");
@LogLines = <LOGFILE>;
close LOGFILE;

my $PlotInfo;

foreach my $LogLine (@LogLines) {
	
	my ($LineType, $LineContents) = split(/\|/,$LogLine);

	$LineContents =~ s/\n//;

	if($LineType eq 'Log') { $PlotInfo->{"log"} .= $LineContents . "\n"; }
	else {

		if($LineType eq 'GPS Latitude') { $PlotInfo->{"gps"}->{"lat"} = $LineContents; }
		elsif($LineType eq 'GPS Longitude') { $PlotInfo->{"gps"}->{"long"} = $LineContents; }
		$PlotInfo->{"params"} .= "<b>$LineType: </b>$LineContents<br />";

	}

}

my $json = JSON->new->allow_nonref;

print $json->encode($PlotInfo);

