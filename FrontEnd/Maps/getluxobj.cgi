#!/usr/bin/perl

use CGI qw(:standard);
use CGI::Carp qw(fatalsToBrowser);
use JSON;




print header('application/json');

my $Run = "0";

if(param('run')) { $Run = param('run'); }

my $LuxFileNumber = 0;

my @LuxFileName;

if(param('no')) { $LuxFileNumber = param('no'); $LuxFileName = $LuxFileNumber . ".luxobj"; }
else {

opendir(LUXDIR, "../../TestCode/Control/RunFiles/$Run/luxobj/");
my @LuxFiles = readdir(LUXDIR);
@LuxFiles = sort(@LuxFiles);
closedir(LUXDIR);
$LuxFileName = $LuxFiles[${@LuxFiles} - 2];
}

open (LUXFILE, "../../TestCode/Control/RunFiles/$Run/luxobj/$LuxFileName");
my @LogLines = <LUXFILE>;
close LUXFILE;

my $PlotInfo;

@LogLines = sort(@LogLines);

foreach my $LogLine (@LogLines) {
	
	#my ($LineType, $LineContents) = split(/|/,$LogLine);


	push(@{$PlotInfo->{"objdata"}}, $LogLine);

}

my $json = JSON->new->allow_nonref;

print $json->encode($PlotInfo);

