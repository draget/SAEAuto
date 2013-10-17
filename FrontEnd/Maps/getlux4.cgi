#!/usr/bin/perl

use CGI qw(:standard);
use CGI::Carp qw(fatalsToBrowser);
use JSON;
use Time::HiRes qw(usleep);


print header('application/json');

my $Run = "0";

if(param('run')) { $Run = param('run'); }

my $Layers = "All";

if(param('layer') ne '') { $Layers = param('layer'); }

my $LuxFileNumber = 0;

my @LuxFileName;

if(param('no')) { $LuxFileNumber = param('no'); $LuxFileName = "../../Control/RunFiles/$Run/luxscan/$LuxFileNumber.lux"; }
else { 

	my $i = 0;
	while(-e "../../Control/ramdisk/ibeo.lck") { if($i > 1000) { last; } usleep(1000); $i++; }

	$LuxFileName = "../../Control/ramdisk/current.lux"; 
	
	}


#print $LuxFileName . "\n";

open (LUXFILE, $LuxFileName);
@LuxLines = <LUXFILE>;
close LUXFILE;

my $PlotInfo;

foreach my $LuxLine (@LuxLines) {
	
	my @LuxLineParts = split(/,/,$LuxLine);

	if($LuxLineParts[0] eq "R") { 
		push(@{$PlotInfo->{"roadedge"}},[$LuxLineParts[1]*1.0,$LuxLineParts[1]*$LuxLineParts[3] + $LuxLineParts[4], 'white']); 
		push(@{$PlotInfo->{"roadedge"}},[$LuxLineParts[2]*1.0,$LuxLineParts[2]*$LuxLineParts[3] + $LuxLineParts[4], 'white']); 	
	}	
	elsif($LuxLineParts[0] == 0 && ($Layers eq '0' || $Layers eq 'All')) { 	push(@{$PlotInfo->{"data"}},[$LuxLineParts[2]*360/11520,$LuxLineParts[3]/100.0, 'red']); }
	elsif($LuxLineParts[0] == 1 && ($Layers eq '1' || $Layers eq 'All')) { 	push(@{$PlotInfo->{"data"}},[$LuxLineParts[2]*360/11520,$LuxLineParts[3]/100.0, 'orange']); }
	elsif($LuxLineParts[0] == 2 && ($Layers eq '2' || $Layers eq 'All')) { 	push(@{$PlotInfo->{"data"}},[$LuxLineParts[2]*360/11520,$LuxLineParts[3]/100.0, 'yellow']); }
	elsif($LuxLineParts[0] == 3 && ($Layers eq '3' || $Layers eq 'All')) { 	push(@{$PlotInfo->{"data"}},[$LuxLineParts[2]*360/11520,$LuxLineParts[3]/100.0, 'green']); }
	else { next; }

}

$LuxObjFilename = $LuxFileName;

$LuxObjFilename =~ s/(luxscan|lux)/luxobj/g;

open (LUXFILE, "$LuxObjFilename");
my @LogLines = <LUXFILE>;
close LUXFILE;

@LogLines = sort(@LogLines);

foreach my $LogLine (@LogLines) {
	
	$LogLine =~ s/\n//;

	#my ($LineType, $LineContents) = split(/|/,$LogLine);
	push(@{$PlotInfo->{"objdata"}}, $LogLine);

}


my $json = JSON->new->allow_nonref;

print $json->encode($PlotInfo);

