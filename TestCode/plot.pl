#!/usr/bin/perl

use lib './perllib';

use Graphics::GnuplotIF;
use List::Util qw(max);
use POSIX qw(ceil);
use Tk;
use Time::HiRes qw(usleep);
use Tk::JComboBox;
use Tk::JPEG;
use DateTime::Format::W3CDTF;
use Tk::Pane;

our $GuiParams;

$GuiParams->{'url'} = "http://192.168.2.3/saedata";

our $MainWindow = MainWindow->new();

$MainWindow->geometry( "1280x750" );

our $MainPane = $MainWindow->Scrolled(Pane, 
        -scrollbars => 'soe',
        -sticky => 'we',
        -gridded => 'y'
    )->pack(-expand => 1, -fill => both);

$MainPane->Label(-text => "URL Base:")->grid(-row => 0, -column => 0);
$MainPane->Entry(-textvariable => \$GuiParams->{'url'}, -width => 30)->grid(-row => 0, -column => 1);

$MainPane->Label(-text => "Data Run id:")->grid(-row => 1, -column => 0);
$MainPane->Entry(-textvariable => \$GuiParams->{'run'}, -width => 30)->grid(-row => 1, -column => 1);

$MainPane->Button(-text => "Download Run Data", -command => \&Download)->grid(-row => 2, -column => 0, -columnspan => 2);

$MainPane->Label(-text => "Current Frame:")->grid(-row => 1, -column => 2);
$GuiParams->{'FrameLabel'} = $MainPane->Entry(-text => '0')->grid(-row => 1, -column => 3);

$MainPane->Button(-text => "Prev Frame", -command => [\&ChangeFrame, -1])->grid(-row => 3, -column => 0);
$MainPane->Button(-text => "Next Frame", -command => [\&ChangeFrame, 1])->grid(-row => 3, -column => 1);

$MainPane->Button(-text => "Play", -command => [\&Play])->grid(-row => 3, -column => 2);
$MainPane->Button(-text => "Stop", -command => [\&StopPlayer])->grid(-row => 3, -column => 3);

$MainPane->Label(-text => "Layer:")->grid(-row => 0, -column => 2);
$GuiParams->{'LayerCombo'} = $MainPane->JComboBox(-selectcommand => \&ShowLuxPlot, -choices => [ 
         { -name => 'Zero',  -value => '0' },
         { -name => 'One',   -value => '1' },
         { -name => 'Two',  -value => '2' },
         { -name => 'Three', -value => '3' },
         ])->grid(-row => 0, -column => 3);
$GuiParams->{'LayerCombo'}->setSelectedIndex(0);

$GuiParams->{'PlotLabel'} = $MainPane->Label()->grid(-row => 4, -column => 0, -columnspan => 2);
$GuiParams->{'PlotImage'} = $MainPane->Photo(-file => "start.gif");
$GuiParams->{'PlotLabel'}->configure(-image => $GuiParams->{'PlotImage'});

$GuiParams->{'PhotoLabel'} = $MainPane->Label()->grid(-row => 4, -column => 2, -columnspan => 2);
$GuiParams->{'PhotoImage'} = $MainPane->Photo(-file => "start.jpeg");
$GuiParams->{'PhotoLabel'}->configure(-image => $GuiParams->{'PhotoImage'});

$MainPane->Label(-text => "Time:")->grid(-row => 5, -column => 0);
$GuiParams->{'TimeLabel'} = $MainPane->Label()->grid(-row => 5, -column => 1);

$MainPane->Label(-text => "Lat:")->grid(-row => 6, -column => 0);
$GuiParams->{'LatLabel'} = $MainPane->Label()->grid(-row => 6, -column => 1);

$MainPane->Label(-text => "Long:")->grid(-row => 7, -column => 0);
$GuiParams->{'LongLabel'} = $MainPane->Label()->grid(-row => 7, -column => 1);

$MainPane->Label(-text => "Speed:")->grid(-row => 8, -column => 0);
$GuiParams->{'SpeedLabel'} = $MainPane->Label()->grid(-row => 8, -column => 1);

$GuiParams->{'GPSLabel'} = $MainPane->Label()->grid(-row => 5, -column => 2, -columnspan => 2, -rowspan => 4);
$GuiParams->{'GPSImage'} = $MainPane->Photo(-file => "");
$GuiParams->{'GPSLabel'}->configure(-image => $GuiParams->{'GPSImage'});

MainLoop;

sub Play {

unless($GuiParams->{'STOP'} == 1) {
	$GuiParams->{'FrameLabel'}->configure(-text => $GuiParams->{'FrameLabel'}->cget('-text') + 1);
	if(ShowLuxPlot()) {
		$MainWindow->after(200, \&Play);
	}
}

}

sub StopPlayer {

$GuiParams->{'STOP'} = 1;

$MainWindow->after(200, sub { $GuiParams->{'STOP'} = 0; });

}

sub ChangeFrame {

my $Increment = shift;

if($GuiParams->{'FrameLabel'}->cget('-text') == 0 && $Increment < 0) { return; }

$GuiParams->{'FrameLabel'}->configure(-text => $GuiParams->{'FrameLabel'}->cget('-text') + $Increment);

ShowLuxPlot();

}


sub Download {

system("wget -r -l1 -P ./rundata/ $GuiParams->{'url'}/run$GuiParams->{'run'}");

}

sub ShowLuxPlot {

my $Frame = $GuiParams->{'FrameLabel'}->cget('-text');

$GuiParams->{'url'} =~ /^http\:\/\/(.+)$/;

my $TruncatedURL = $1;

my $ImageId = '';

print "./rundata/$TruncatedURL/run" . $GuiParams->{'run'} . "/$Frame.lux" . "\n";


if($Frame < 10) { $ImageId = "000$Frame"; }
elsif($Frame < 100) { $ImageId = "00$Frame"; }
elsif($Frame < 1000) { $ImageId = "0$Frame"; }
else { $ImageId = "$Frame"; }

if((! -e "./rundata/$TruncatedURL/run" . $GuiParams->{'run'} . "/$Frame.lux") && (! -e "./rundata/$TruncatedURL/run" . $GuiParams->{'run'} . "/$ImageId.jpeg")) { 
	unless($Frame == 0) { ChangeFrame(-1); ShowLuxPlot(); }
	return 0; 
}

print "./rundata/$TruncatedURL/run" . $GuiParams->{'run'} . "/$Frame.lux";

open (LUXFILE,"./rundata/$TruncatedURL/run" . $GuiParams->{'run'} . "/$Frame.lux");
my @LuxLines = <LUXFILE>;
close LUXFILE;

my @PlotRadius;
my @PlotAngle;

foreach my $LuxLine (@LuxLines) {

	my ($LayerEcho, $Flags, $HAngle, $RDist, $EPW, undef) = split(/,/, $LuxLine);

	if($LayerEcho == $GuiParams->{'LayerCombo'}->getSelectedValue()) {
		push(@PlotRadius, $RDist/100);
		push(@PlotAngle, $HAngle*360/11520 + 90);
	}

}

my $Plot = Graphics::GnuplotIF->new(title => "Frame $Frame Layer " . $GuiParams->{'LayerCombo'}->getSelectedValue(), style => 'points');

my $ScaleIncrement;

if(max(@PlotRadius) < 6) { $ScaleIncrement = 2; }
elsif(max(@PlotRadius) < 15) { $ScaleIncrement = 5; }
else { $ScaleIncrement = 10; }

my $ScaleFactor = $ScaleIncrement*ceil(max(@PlotRadius)/$ScaleIncrement);

$Plot->gnuplot_cmd('set terminal gif size 640,480', 'set output "curplot.gif"', 'set polar', 'set size ratio 0.5', 'set angles degrees', 'set grid polar 45', 'set yrange [0:' . $ScaleFactor . ']', 'set xrange [-' . $ScaleFactor . ':' . $ScaleFactor . ']');

$Plot->gnuplot_plot_xy(\@PlotAngle, \@PlotRadius);

if( -e "./rundata/$TruncatedURL/run" . $GuiParams->{'run'} . "/gps.log") {

open(GPSFILE, "./rundata/$TruncatedURL/run" . $GuiParams->{'run'} . "/gps.log");
my @GPSLines = <GPSFILE>;
close GPSFILE;

my @Lats, @Longs, $CurLong, $CurLat;

foreach my $GPSLine (@GPSLines) {

my @LineData = split(/,/, $GPSLine);

	if($LineData[0] == $Frame) {

	my $W3C = DateTime::Format::W3CDTF->new;
	my $DT = $W3C->parse_datetime($LineData[1]);

	$GuiParams->{'TimeLabel'}->configure(-text => $DT->add(hours => 8)->hms);
	$GuiParams->{'LatLabel'}->configure(-text => $LineData[2]);
	$GuiParams->{'LongLabel'}->configure(-text => $LineData[3]);
	$GuiParams->{'SpeedLabel'}->configure(-text => $LineData[4]);

	$CurLat = $LineData[2];
	$CurLong = $LineData[3];

	}

	else { push(@Lats, $LineData[2]); push(@Longs, $LineData[3]); }

}

my $GPSPlot = Graphics::GnuplotIF->new(title => "GPS Plot Frame: $Frame", style => 'points');

$GPSPlot->gnuplot_cmd('set terminal gif size 640,350', 'set output "curgps.gif"');

$GPSPlot->gnuplot_plot_xy1($CurLong, $CurLat, \@Longs, \@Lats);

}

usleep(50000);




$GuiParams->{'PhotoImage'}->blank();

if(-e "./rundata/$TruncatedURL/run" . $GuiParams->{'run'} . "/$ImageId.jpeg") {

$GuiParams->{'PhotoImage'}->read("./rundata/$TruncatedURL/run" . $GuiParams->{'run'} . "/$ImageId.jpeg");

}

$GuiParams->{'PlotImage'}->blank();
$GuiParams->{'PlotImage'}->read("curplot.gif");

$GuiParams->{'GPSImage'}->blank();
$GuiParams->{'GPSImage'}->read("curgps.gif");

return 1;

}
