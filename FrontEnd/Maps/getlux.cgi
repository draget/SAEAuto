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

opendir(LUXDIR, "/opt/SAE/git/SAEAuto/TestCode/Control/RunFiles/$Run");
foreach my $FileName (readdir(LUXDIR)) {

	if($FileName !~ /\.lux$/i) { next; }
	else {

	$FileName =~ /^([0-9\.]+)\.lux$/i;

	if($1 > $LuxFileNumber) { $LuxFileNumber = $1; }

	}


}
closedir(LUXDIR);

}

open (LUXFILE, "/opt/SAE/git/SAEAuto/TestCode/Control/RunFiles/$Run/$LuxFileNumber.lux");
@LuxLines = <LUXFILE>;
close LUXFILE;

my $PlotInfo;

foreach my $LuxLine (@LuxLines) {
	
	my @LuxLineParts = split(/,/,$LuxLine);

	my $Colour;

	if($LuxLineParts[0] == 0) { $Colour = 'red'; }
	elsif($LuxLineParts[0] == 1) { $Colour = 'blue'; }
	elsif($LuxLineParts[0] == 2) { $Colour = 'green'; }
	elsif($LuxLineParts[0] == 3) { $Colour = 'purple'; }
	else { next; }
	

	push(@{$PlotInfo->{"data"}},[$LuxLineParts[2]*360/11520,$LuxLineParts[3]*1.0, $Colour]);

}

my $json = JSON->new->allow_nonref;

print $json->encode($PlotInfo);

