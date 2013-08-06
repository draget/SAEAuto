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
	
	my ($LineType, $LineContents) = split(/|/,$LogLine);

	if($LineType eq 'GPS Latitude') { $PlotInfo->{"gps"}->{"lat"} = $LineContents; }
	elsif($LineType eq 'GPS Longitude') { $PlotInfo->{"gps"}->{"long"} = $LineContents; }

	$PlotInfo->{"log"} .= $LogLine;

}

my $json = JSON->new->allow_nonref;

print $json->encode($PlotInfo);

