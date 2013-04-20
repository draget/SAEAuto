#!/usr/bin/perl

use lib './perllib';

use GPS::NMEA;
use Time::HiRes qw(usleep);

my $i = 0;
my $LastTime = -1;

my $GPS;

print "Opening GPS...\n";

unless($GPS = GPS::NMEA->new(Port => '/dev/serial/by-id/usb-MTK_GPS_Receiver-if01', Baud => '115200')) { die "Coudn't connect to GPS..." };

print "Checking for a fix...\n";

my @FixInfo = $GPS->get_fix_info();

unless($FixInfo[0] > 0) { die "No fix..."; }

print "Setting up directory...\n";

opendir(DIR, "./data/");

my $Run = 0;

while(readdir(DIR)) {
	
	if($_ =~ /^run([0-9]+)$/) {
		if($1 >= $Run) { $Run = $1 + 1; }
	}

}

system("mkdir ./data/run$Run");

print "Run $Run Running...\n";

while(1) {

	my @GPSData = $GPS->get_location_speed_time();

	$GPSData[5] =~ /^([0-9]{2}):([0-9]{2}):([0-9]{2}.[0-9]{3})$/;

	my $CurTime = $1*3600 + $2*60 + $3;

	if($CurTime > $LastTime) {

		open (FILE, ">./data/run$Run/$i.gps");
		print FILE join(',', @GPSData);
		close FILE;

		system("./IBEO ./data/run$Run/$i.lux");

		$i++;
		$LastTime = $GPSData[0];

	}

usleep(50000);

}
