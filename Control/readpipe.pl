#!/usr/bin/perl

my $fifo_file = "./IPC_FIFO_RX";

my $fifo_fh;
open($fifo_fh, "+< $fifo_file") or die "The FIFO file \"$fifo_file\" is missing, and this program can't run without it.";

# just keep reading from the fifo and processing the events we read
while (<$fifo_fh>)
{
 print $_ . "\n";
}

# should never really come down here ...
close $fifo_fh;
exit(0);
