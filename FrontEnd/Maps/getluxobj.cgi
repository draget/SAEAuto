#!/usr/bin/perl

use CGI qw(:standard);
use CGI::Carp qw(fatalsToBrowser);
use JSON;




print header('application/json');

my $Run = "0";

if(param('run')) { $Run = param('run'); }

my $LuxFileNumber = 0;

if(param('no')) { $LuxFileNumber = param('no'); }
else {

	opendir(LUXDIR, "../../TestCode/Control/RunFiles/$Run");
	foreach my $FileName (readdir(LUXDIR)) {

		if($FileName !~ /\.luxobj$/i) { next; }
		else {

		$FileName =~ /^([0-9\.]+)\.luxobj$/i;

		if($1 > $LuxFileNumber) { $LuxFileNumber = $1; }

		}


	}
	closedir(LUXDIR);

}

open (LUXFILE, "../../TestCode/Control/RunFiles/$Run/$LuxFileNumber.luxobj");
my @LogLines = <LUXFILE>;
close LUXFILE;

my $PlotInfo;

@LogLines = sort(@LogLines);

foreach my $LogLine (@LogLines) {
	
	my ($LineType, $LineContents) = split(/|/,$LogLine);


	push(@{$PlotInfo->{"objdata"}}, $LogLine);

}

my $json = JSON->new->allow_nonref;

print $json->encode($PlotInfo);

