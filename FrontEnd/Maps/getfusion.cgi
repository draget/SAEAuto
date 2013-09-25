#!/usr/bin/perl

use CGI qw(:standard);
use CGI::Carp qw(fatalsToBrowser);
use JSON;
use Time::HiRes qw(usleep);


print header('application/json');

my $Run = "0";

if(param('run')) { $Run = param('run'); }

open (FUSIONFILE, "../../TestCode/Control/RunFiles/$Run/fusion.log");
my @FusionLines = <FUSIONFILE>;
close FUSIONFILE;

shift @FusionLines;

my $LastLineType = "";

foreach my $FusionLine (@FusionLines) {

	$FusionLine =~ s/\n//;
	my ($LineType, @LineContents) = split(/,/,$FusionLine);


	if($LineType eq 'D') { next; }
	elsif($LineType eq "UP") { 
		push(@{$PlotInfo->{"gpsposdata"}},[$LineContents[1]*1.0, $LineContents[2]*1.0, "red"]);
	}
	elsif($LineType eq "P" && $LastLineType =~ /^U/) { 
		push(@{$PlotInfo->{"posdata"}},[$LineContents[1]*1.0, $LineContents[2]*1.0, "blue"]);
	}
	elsif($LineType eq "P") { 
		push(@{$PlotInfo->{"posdata"}},[$LineContents[1]*1.0, $LineContents[2]*1.0, "green"]);
	}
	elsif($LineType eq "T") { 
		push(@{$PlotInfo->{"fusedheadingdata"}},[$LineContents[0]*1.0, $LineContents[1]*1.0, "blue"]);
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

	$LastLineType = $LineType;

}


open (IMUFILE, "../../TestCode/Control/RunFiles/$Run/imu.log");
my @IMULines = <IMUFILE>;
close IMUFILE;

shift @IMULines;

foreach my $IMULine (@IMULines) {

	$IMULine =~ s/\n//;
	my @LineContents = split(/,/,$IMULine);

	push(@{$PlotInfo->{"headingdata"}},[$LineContents[0]*1.0, $LineContents[1]*1.0, "green"]);

}


open (GPSFILE, "../../TestCode/Control/RunFiles/$Run/gps.log");
my @GPSLines = <GPSFILE>;
close GPSFILE;

shift @GPSLines;

foreach my $GPSLine (@GPSLines) {

	$GPSLine =~ s/\n//;
	my ($LineType, @LineContents) = split(/,/,$GPSLine);

	if($LineType eq "T") { 
		push(@{$PlotInfo->{"headingdata"}},[$LineContents[0]*1.0, $LineContents[1]*1.0, "red"]);
	}

}

open (AUTOLOG, "../../TestCode/Control/RunFiles/$Run/autolog.txt");
my @AutoLines = <AUTOLOG>;
close AUTOLOG;

shift @AutoLines;

foreach my $AutoLine (@AutoLines) {

	$AutoLine =~ s/\n//;
	my ($LineType, @LineContents) = split(/,/,$AutoLine);

	if($LineType eq "SS") { 
		push(@{$PlotInfo->{"steervalues"}},[$LineContents[0]*1.0, $LineContents[1]*1.0, "purple"]);
	}
	elsif($LineType eq "DB") {
		push(@{$PlotInfo->{"desiredheading"}},[$LineContents[0]*1.0, $LineContents[1]*1.0, "red"]);
	}

}



my $json = JSON->new->allow_nonref;

print $json->encode($PlotInfo);

