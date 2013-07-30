#!/usr/bin/perl

use CGI qw(:standard);
use CGI::Carp qw(fatalsToBrowser);
use JSON;


print header('application/json');

open (LUXFILE, "test.lux");
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
	

	push(@{$PlotInfo->{"data"}},[$LuxLineParts[2]*360/11520,$LuxLineParts[3]*1.0, $Colour]);

}

my $json = JSON->new->allow_nonref;

print $json->encode($PlotInfo);

