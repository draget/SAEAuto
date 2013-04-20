#!/usr/bin/perl

use lib './perllib';

use Time::HiRes;

$SIG{'INT'} = sub { close FILE; exit(0); };

my $i = 0;



while(1) {

sleep(1);

	system("./IBEO ./rundata/local/saedata/run2/$i.lux &");

print $i;

	$i++;

}

close FILE;

