#!/usr/bin/perl

use CGI qw(:standard);
use CGI::Carp qw(fatalsToBrowser);
use JSON;
use Time::HiRes qw(usleep);


print header('application/json');

my $Run = "0";

if(param('run')) { $Run = param('run'); }

open (MAPFILE, "../../TestCode/Control/RunFiles/$Run/fusion.log");
my @FusionLines = <MAPFILE>;
close MAPFILE;


foreach my $FusionLine (@FusionLines) {

	$FusionLine =~ s/\n//;
	my ($LineType, @LineContents) = split(/,/,$FusionLine);

	my $Colour;

	if($LineType eq 'D') { next; }
	elsif($LineType eq "UP") { 
		push(@{$PlotInfo->{"posdata"}},[$LineContents[1]*1.0, $LineContents[2]*1.0, "red"]);
	}
	elsif($LineType eq "P") { 
		push(@{$PlotInfo->{"posdata"}},[$LineContents[1]*1.0, $LineContents[2]*1.0, "blue"]);
	}

	elsif($LineType eq "T") { 
		push(@{$PlotInfo->{"headingdata"}},[$LineContents[0]*1.0, $LineContents[1]*1.0, "blue"]);
	}

	elsif($LineType eq "V") { 
		push(@{$PlotInfo->{"veldata"}},[$LineContents[0]*1.0, $LineContents[1]*1.0, "blue"]);
		push(@{$PlotInfo->{"veldata"}},[$LineContents[0]*1.0, $LineContents[2]*1.0, "orange"]);
		push(@{$PlotInfo->{"speeddata"}},[$LineContents[0]*1.0, sqrt($LineContents[2]**2 + $LineContents[1]**2), "blue"]);
	}	
	elsif($LineType eq "UV") { 
		push(@{$PlotInfo->{"veldata"}},[$LineContents[0]*1.0, $LineContents[1]*1.0, "green"]);
		push(@{$PlotInfo->{"veldata"}},[$LineContents[0]*1.0, $LineContents[2]*1.0, "red"]);
		push(@{$PlotInfo->{"speeddata"}},[$LineContents[0]*1.0, sqrt($LineContents[2]**2 + $LineContents[1]**2), "red"]);
	}



}

my $json = JSON->new->allow_nonref;

print $json->encode($PlotInfo);

