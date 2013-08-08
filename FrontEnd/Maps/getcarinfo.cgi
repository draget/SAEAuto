#!/usr/bin/perl

use CGI qw(:standard);
use CGI::Carp qw(fatalsToBrowser);
use JSON;


print header('application/json');


open (LOGFILE, "../../TestCode/Control/weblog.txt");
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

