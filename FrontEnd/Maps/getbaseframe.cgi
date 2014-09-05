#!/usr/bin/perl

use CGI qw(:standard);
use CGI::Carp qw(fatalsToBrowser);
use JSON;
use Time::HiRes qw(usleep);


print header('application/json');

my $i = 0;
while(-e "../../Control/ramdisk/baseframe.txt_LOCK") { if($i > 1000) { last; } usleep(1000); $i++; }

open (BFFILE, "../../Control/ramdisk/baseframe.txt");
my @BFLines = <BFFILE>;
close BFFILE;

my $BFInfo;


foreach my $BFLine (@BFLines) {

	$BFLine =~ s/\n//;
	my ($LineType, @LineContents) = split(/,/,$BFLine);

	my $Colour;

	if($LineType =~ /^[0-9]+$/) { 
		$Colour = "blue";
		push(@{$BFInfo->{"mapdata"}},[$LineContents[0]*1.0, $LineContents[1]*1.0, $Colour]);
	}

	

}

my $json = JSON->new->allow_nonref;
print $json->encode($BFInfo);



