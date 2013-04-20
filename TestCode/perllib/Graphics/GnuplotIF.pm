#
#===============================================================================
#
#         FILE:  GnuplotIF.pm
#
#  DESCRIPTION:  A simple and easy to use Perl interface to gnuplot.
#                (see POD below)
#        FILES:  ---
#         BUGS:  ---
#        NOTES:  ---
#       AUTHOR:  Dr.-Ing. Fritz Mehner (Mn), <mehner@fh-swf.de>
#      COMPANY:  FH Suedwestfalen, Iserlohn
#      VERSION:  see $VERSION below
#      CREATED:  16.07.2005 13:43:11 CEST
#     REVISION:  $Id: GnuplotIF.pm,v 1.1.1.1 2007/06/07 07:14:27 mehner Exp $
#===============================================================================

package Graphics::GnuplotIF;

use strict;
use warnings;
use IO::Handle;

use version; our $VERSION     = qv('1.2.0');    # version number

use base qw(Exporter);

#=======================================================================
#  NON=EXPORTED PACKAGE GLOBALS
#=======================================================================

my  %linestyles =                               # allowed line styles
(
    boxes       => q{},
    dots        => q{},
    filledcurves=> q{},
    fsteps      => q{},
    histeps     => q{},
    impulses    => q{},
    lines       => q{},
    linespoints => q{},
    points      => q{},
    steps       => q{},
);

my  $object_number   = 0;                       # number of created objects

#
#  Code common to gnuplot_plot_xy and gnuplot_plot_y to allow user-
#  specified titles set by gnuplot_set_plot_titles.  This code assumes the
#  plot titles were all set in the command to the literal text "<PLOT_TITLE>",
#  without any surrounding quotes.  This function replaces that text.
#
my $private_apply_plot_titles = sub {
    my ($self, $cmd_ref) = @_;
    my $default_plot_title = q{-};              # Title if user did not specify one
    if (defined $self->{plot_titles} ) {
        #  Substitute each plot title sequentially with the user-supplied value
        for my $plot_title (@{$self->{plot_titles}}) {
            if ( !defined $plot_title ) {
                $plot_title = $default_plot_title;
            }
            ${$cmd_ref} =~ s/title <PLOT_TITLE>/title "$plot_title"/;
        }
    }

    # Substitute any plot titles we did not already catch globally
    ${$cmd_ref} =~ s/title <PLOT_TITLE>/title "$default_plot_title"/g;
};

#=======================================================================
#  MODULE CODE
#=======================================================================

#===  FUNCTION  ================================================================
#         NAME:  new
#  DESCRIPTION:  constructor
# PARAMETER  1:  anonymous hash containing some plot parameter (defaults shown):
#                 style   => "lines"
#                 title   => "",
#                 xlabel  => "",
#                 ylabel  => "",
#                 xrange  => [],
#                 yrange  => [],
#      RETURNS:  object reference
#===============================================================================
sub new {

    if ( ! $ENV{'DISPLAY'} ) {
        die "Graphics::GnuplotIF : cannot find environment variable DISPLAY \n"
    }

    my  $PIPE;
    open $PIPE, '|- ', 'gnuplot'
        or die "\n$0 : failed to open pipe to \"gnuplot\" : $!\n";
    $PIPE->autoflush(1);

    my  ( $class, %args ) = @_;

    my  $self   = {
        style         => 'lines',
        title         => q{},
        xlabel        => q{},
        ylabel        => q{},
        xrange        => [],
        yrange        => [],
        %args,
        __pipehandle  => $PIPE,
        __pausetime   => -1,
        __pausemess   => q{},
        __objectnr    => ++$object_number,
    };

    bless $self, $class ;

    $self->gnuplot_set_style (   $self->{style }  );
    $self->gnuplot_set_title (   $self->{title }  );
    $self->gnuplot_set_xlabel(   $self->{xlabel}  );
    $self->gnuplot_set_ylabel(   $self->{ylabel}  );
    $self->gnuplot_set_xrange( @{$self->{xrange}} );
    $self->gnuplot_set_yrange( @{$self->{yrange}} );

    return $self;
} # ----------  end of subroutine new  ----------

#---------------------------------------------------------------------------
#  DESTROY
#---------------------------------------------------------------------------
sub DESTROY {
    my  $self = shift;
    close $self->{__pipehandle} or
    print { *STDERR } "Graphics::GnuplotIF ($self->{__objectnr}):  "
    ."problem closing communication to gnuplot\n";
    return;
} # ----------  end of subroutine DESTROY  ----------

#===  CLASS METHOD  ============================================================
#         NAME:  gnuplot_plot_y
#      PURPOSE:  Plot one or more arrays over 0, 1, 2, 3, ...
#   PARAMETERS:  array reference(s)
#      RETURNS:  ---
#===============================================================================
sub gnuplot_plot_y {
    my  $parnr  = 0;
    my  ( $self, @yref )  = @_;
    my  $cmd  = " '-' with $self->{style} title <PLOT_TITLE>," x (scalar @yref);
    $cmd =~ s/,$//;
    $self->$private_apply_plot_titles(\$cmd); # Honor gnuplot_set_plot_titles
    return if $cmd eq q{};

    print { $self->{__pipehandle}} "plot $cmd\n";

    foreach my $item ( @yref ) {
        $parnr++;
        die "Graphics::GnuplotIF ($self->{__objectnr}): gnuplot_plot_y : $parnr. "
        ."argument not an array reference\n"
        if ref($item) ne 'ARRAY';
        print { $self->{__pipehandle}} join( "\n", @{$item}), "\ne\n";
    }       # -----  end foreach  -----
    return;
} # ----------  end of subroutine gnuplot_plot_y  ----------

#===  CLASS METHOD  ============================================================
#         NAME:  gnuplot_plot_xy
#      PURPOSE:  x-y-plot(s)
#   PARAMETERS:  1. array reference : x-values
#                2. array reference : y-values
#                 ...
#      RETURNS:  ---
#  DESCRIPTION:  Takes two or more array references. The first array is assumed
#                to contain the x-values for the following function values.
#===============================================================================
sub gnuplot_plot_xy {
    my  $parnr  = 1;
    my  ( $self, $xref, @yref )   = @_;
    my  $cmd  = " '-' using 1:2 with $self->{style} title <PLOT_TITLE>," x (scalar @yref);
    $cmd =~ s/,$//;
    $self->$private_apply_plot_titles(\$cmd); # Honor gnuplot_set_plot_titles
    return if $cmd eq q{};

    print { $self->{__pipehandle}} "plot $cmd\n";

    die "Graphics::GnuplotIF ($self->{__objectnr}): gnuplot_plot_xy : $parnr. "
    ."argument not an array reference\n"
    if ref($xref) ne 'ARRAY';

    foreach my $j ( 0..$#yref ) {
        $parnr++;
        die "Graphics::GnuplotIF ($self->{__objectnr}): gnuplot_plot_xy - "
        ."$parnr. argument not an array reference\n"
        if ref($yref[$j]) ne 'ARRAY';

        # there may be lesser y-values than x-values

        my  $min    = $#{$xref} < $#{$yref[$j]}  ?  $#{$xref}  :  $#{$yref[$j]};
        foreach my $i ( 0..$min ) {
            print { $self->{__pipehandle}} "$$xref[$i] $yref[$j]->[$i]\n";
        }
        print { $self->{__pipehandle}} "e\n";
    }
    return;
} # ----------  end of subroutine gnuplot_plot_xy  ----------

#===  CLASS METHOD  ============================================================
#         NAME:  gnuplot_plot_xy1
#      PURPOSE:  x-y-plot(s)
#   PARAMETERS:  1. array reference : x-values
#                2. array reference : y-values
#                 ...
#      RETURNS:  ---
#  DESCRIPTION:  Takes two or more array references. The first array is assumed
#                to contain the x-values for the following function values.
#===============================================================================
sub gnuplot_plot_xy1 {
    my  $parnr  = 1;
    my  ( $self, $x1, $y1,$xref, $yref )   = @_;
    my  $cmd  = " '-' using 1:2 with $self->{style} title <PLOT_TITLE>," x 2;
    $cmd =~ s/,$//;
    $self->$private_apply_plot_titles(\$cmd); # Honor gnuplot_set_plot_titles
    return if $cmd eq q{};

    print { $self->{__pipehandle}} "plot $cmd\n";

    die "Graphics::GnuplotIF ($self->{__objectnr}): gnuplot_plot_xy : $parnr. "
    ."argument not an array reference\n"
    if ref($xref) ne 'ARRAY';

    
        die "Graphics::GnuplotIF ($self->{__objectnr}): gnuplot_plot_xy - "
        ."$parnr. argument not an array reference\n"
        if ref($yref) ne 'ARRAY';

        # there may be lesser y-values than x-values

        my  $min    = $#{$xref} < $#{$yref}  ?  $#{$xref}  :  $#{$yref};
        foreach my $i ( 0..$min ) {
            print { $self->{__pipehandle}} "$$xref[$i] $$yref[$i]\n";
        }
        print { $self->{__pipehandle}} "e\n";

print { $self->{__pipehandle}} "$x1 $y1\n";
 print { $self->{__pipehandle}} "e\n";

    return;
} # ----------  end of subroutine gnuplot_plot_xy  ----------



#===  CLASS METHOD  ============================================================
#         NAME:  gnuplot_plot_equation
#      PURPOSE:  Plot one ore more functions described by strings.
#   PARAMETERS:  strings describing functions
#      RETURNS:  ---
#===============================================================================
sub gnuplot_plot_equation {
    my  ( $self, @equations ) = @_;
    my  $leftside;
    my  @leftside;

    foreach my $equ ( @equations ) {
        print { $self->{__pipehandle} } "$equ\n";
        ( $leftside ) = split /=/, $equ;
        push @leftside, $leftside;
    }       # -----  end foreach  -----
    @leftside = map  {$_." with $self->{style}"} @leftside;
    $leftside = join ', ', @leftside;
    return if $leftside eq q{};
    print { $self->{__pipehandle}} "plot $leftside\n";
    return;
} # ----------  end of subroutine gnuplot_plot_equation  ----------

#===  CLASS METHOD  ============================================================
#         NAME:  gnuplot_pause
#      PURPOSE:  Wait a specified amount of time.
#   PARAMETERS:  1. parameter (optional): time value (seconds):
#                   -1  wait for a carriage return
#                    0  do not wait
#                   >0  wait the specified number of seconds
#                2. parameter (optional): text
#                   message to display
#      RETURNS:  ---
#===============================================================================
sub gnuplot_pause {
    my  ( $self, $pause, $message ) = @_;
    if ( defined $pause && $pause =~ m/[+-]?(\d+|\d+\.\d*|\d*\.\d+)/) {
        $self->{__pausetime}    = $pause;
    }
    if ( defined $message && $message ne q{} ) {
        $self->{__pausemess}    = "\"$message\"";
    }
    if ( $self->{__pausetime} <= 0 ) {
        print "Graphics::GnuplotIF ($self->{__objectnr}):  $self->{__pausemess}  --  "
        ."hit RETURN to continue \n";
        my $dummy = <STDIN>;                           # hit RETURN to go on
    }
    else {
        print "Graphics::GnuplotIF ($self->{__objectnr}):  $self->{__pausemess}  --  "
        ."wait $self->{__pausetime} second(s) \n";
        print { $self->{__pipehandle}} "pause $self->{__pausetime} \n";
    }
    return;
}   # ----------  end of subroutine gnuplot_pause  ----------

#===  CLASS METHOD  ============================================================
#         NAME:  gnuplot_cmd
#      PURPOSE:  Pass on one or more Gnuplot commands.
#   PARAMETERS:  string(s)
#      RETURNS:  ---
#===============================================================================
sub gnuplot_cmd {
    my  ($self, @commands)  = @_;
    @commands = map {$_."\n"} @commands;
    print { $self->{__pipehandle}} @commands;
    return;
} # ----------  end of subroutine gnuplot_cmd  ----------

#===  CLASS METHOD  ============================================================
#         NAME:  gnuplot_set_plot_titles
#      PURPOSE:  Sets the list of titles used in the key.
#   PARAMETERS:  array of titles
#      RETURNS:  ---
#===============================================================================
sub gnuplot_set_plot_titles {
    my  ( $self, @user_plot_titles )  = @_;

    my @plot_titles = @user_plot_titles;
    $self->{plot_titles} = \@plot_titles;
    return;
}

#===  CLASS METHOD  ============================================================
#         NAME:  gnuplot_reset
#      PURPOSE:  Set all options set with the set command to their 
#                gnuplot default values.
#   PARAMETERS:  ---
#      RETURNS:  ---
#===============================================================================
sub gnuplot_reset {
    my  ($self)  = @_;
    $self->{plot_titles} = undef;
    print { $self->{__pipehandle}} "reset\n";
    return;
} # ----------  end of subroutine gnuplot_reset  ----------

#===  CLASS METHOD  ============================================================
#         NAME:  gnuplot_set_style
#      PURPOSE:  Sets one of the allowed line styles in a plot command.
#   PARAMETERS:  plotstyle (string)
#      RETURNS:  ---
#     SEE ALSO:  new()
#===============================================================================
sub gnuplot_set_style {
    my  $self   = shift;
    my  $style  = shift;
    if ( defined $style && exists $linestyles{$style} ) {
        $self->{style}  = $style;
    }
    return;
} # ----------  end of subroutine gnuplot_set_style  ----------

#===  CLASS METHOD  ============================================================
#         NAME:  gnuplot_set_title
#      PURPOSE:  Sets the plot title.
#   PARAMETERS:  title (string)
#      RETURNS:  ---
#===============================================================================
sub gnuplot_set_title {
    my  ( $self, $title ) = @_;
    if ( defined $title ) {
        print { $self->{__pipehandle}} "set title  \"$title\"\n"
    };
    return;
} # ----------  end of subroutine gnuplot_set_title  ----------

#===  CLASS METHOD  ============================================================
#         NAME:  gnuplot_set_xlabel
#      PURPOSE:  Sets the x axis label.
#   PARAMETERS:  string
#      RETURNS:  ---
#===============================================================================
sub gnuplot_set_xlabel {
    my  ( $self, $xlabel )    = @_;
    if ( defined $xlabel ) {
        print { $self->{__pipehandle}} "set xlabel  \"$xlabel\"\n"
    };
    return;
} # ----------  end of subroutine gnuplot_set_xlabel  ----------

#===  CLASS METHOD  ============================================================
#         NAME:  gnuplot_set_ylabel
#      PURPOSE:  Sets the y axis label.
#   PARAMETERS:  string
#      RETURNS:  ---
#===============================================================================
sub gnuplot_set_ylabel {
    my  ( $self, $ylabel )    = @_;
    if ( defined $ylabel ) {
        print { $self->{__pipehandle}} "set ylabel  \"$ylabel\"\n"
    };
    return;
} # ----------  end of subroutine gnuplot_set_ylabel  ----------

#===  CLASS METHOD  ============================================================
#         NAME:  gnuplot_set_xrange
#      PURPOSE:  Sets the horizontal range that will be displayed.
#   PARAMETERS:  1. parameter: range, left value
#                2. parameter: range, right value
#      RETURNS:  ---
#===============================================================================
sub gnuplot_set_xrange {
    my  ( $self, $xleft, $xright )  = @_;
    if ( defined $xleft && defined $xright ) {
        print { $self->{__pipehandle}} "set xrange [ $xleft : $xright ]\n";
    }
    return;
} # ----------  end of subroutine gnuplot_set_xrange  ----------

#===  CLASS METHOD  ============================================================
#         NAME:  gnuplot_set_yrange
#      PURPOSE:  Sets the vertical range that will be displayed.
#   PARAMETERS:  1. parameter: range, low value
#                2. parameter: range, high value
#      RETURNS:  ---
#===============================================================================
sub gnuplot_set_yrange {
    my  ( $self, $yleft, $yright )  = @_;
    if ( defined $yleft && defined $yright ) {
        print { $self->{__pipehandle}} "set yrange [ $yleft : $yright ]\n";
    }
    return;
} # ----------  end of subroutine gnuplot_set_yrange  ----------

END { }                                         # module clean-up code

1;


__END__
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#  Module Documentation
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

=head1 NAME

Graphics::GnuplotIF - A dynamic Perl interface to gnuplot

=head1 VERSION

This documentation refers to Graphics::GnuplotIF version 1.2

=head1 SYNOPSIS

  use Graphics::GnuplotIF;

  my  @x   = ( -2, -1.50,  -1,  -0.50,  0,  0.50,  1, 1.50, 2 ); # x values
  my  @y1  = (  4,  2.25,   1,   0.25,  0,  0.25,  1, 2.25, 4 ); # function 1
  my  @y2  = (  2,  0.25,  -1,  -1.75, -2, -1.75, -1, 0.25, 2 ); # function 2

  my  $plot1  = Graphics::GnuplotIF->new( title => "line", style => "points" );

  $plot1->gnuplot_plot_y( \@x );              # plot 9 points over 0..8

  $plot1->gnuplot_pause( );                   # hit RETURN to continue

  $plot1->gnuplot_set_title( "parabola" );    # new title
  $plot1->gnuplot_set_style( "lines" );       # new line style

  $plot1->gnuplot_plot_xy( \@x, \@y1, \@y2 ); # rewrite plot1 : y1 and y2 over x

  my  $plot2  = Graphics::GnuplotIF->new;

  $plot2->gnuplot_set_xrange(  0, 4 );        # set x range
  $plot2->gnuplot_set_yrange( -2, 2 );        # set y range
  $plot2->gnuplot_cmd( "set grid" );          # send a gnuplot command
  $plot2->gnuplot_plot_equation(              # 3 equations in one plot
    "y1(x) = sin(x)",
    "y2(x) = cos(x)",
    "y3(x) = sin(x)/x" );

  $plot2->gnuplot_pause( );                   # hit RETURN to continue

  $plot2->gnuplot_plot_equation(              # rewrite plot 2
    "y4(x) = 2*exp(-x)*sin(4*x)" );

  $plot2->gnuplot_pause( );                   # hit RETURN to continue


=head1 DESCRIPTION

Graphics::GnuplotIF is a simple and easy to use dynamic Perl interface to
B<gnuplot>.  B<gnuplot> is a freely available, command-driven graphical display
tool for Unix.  It compiles and works quite well on a number of Unix flavours
as well as other operating systems. 

This module enables sending display requests asynchronously to B<gnuplot>
through simple Perl subroutine calls.

Graphics::GnuplotIF starts B<gnuplot> as a separate process. The plot commands
are send through a I<pipe>. The graphical output from B<gnuplot> will be
displayed immediately.

Several independent plots can be started from one script.  Each plot has its
own pipe.  All pipes will be closed automatically when the script terminates.
The B<gnuplot> processes terminate when the corresponding pipes are closed.
Their graphical output will now disappear.

Graphics::GnuplotIF is similar to C< gnuplot_i >, a C interface to B<gnuplot>
( http://ndevilla.free.fr/gnuplot/ ), and to  C< gnuplot_i++ >, a C++ interface
to B<gnuplot> ( http://jijo.cjb.net/code/cc++ ).


=head1 SUBROUTINES/METHODS

An object of this class represents an interface to a running B<gnuplot>
process.  During the creation of an object such an process will be started for
each such object.  Communication is done through an unidirectional pipe; the
resulting  stream  is  write-only.


=head2 new

The constructor creates a new B<gnuplot> session object, referenced by a
handle:

  $plot1  = Graphics::GnuplotIF->new( );

A few named arguments can be passed as key - value  pairs (here shown with
their default values):

  style   => "lines",     # one of the gnuplot line styles (see below)
  title   => "",          # string
  xlabel  => "x",         # string
  ylabel  => "y",         # string
  xrange  => [],          # array reference; autoscaling, if empty
  xrange  => [],          # array reference; autoscaling, if empty

These attributes are stored in each object.
Allowed line styles are

  boxes     dots   filledcurves  fsteps  histeps
  impulses  lines  linespoints   points  steps

The objects are automatically deleted by a destructor.  The destructor closes
the pipe to the B<gnuplot> process belonging to that object.  The B<gnuplot>
process will also terminate and remove the graphic output.  The termination can
be controlled by the method L<C<gnuplot_pause> | gnuplot_pause> .


=head2  gnuplot_plot_y

  $plot1->gnuplot_plot_y( \@y1, \@y2 );

C<gnuplot_plot_y> takes one or more array references and plots the values over
the x-values 0, 1, 2, 3, ...

=head2 gnuplot_plot_xy

  $plot1->gnuplot_plot_xy( \@x, \@y1, \@y2 );

C<gnuplot_plot_xy> takes two or more array references.  The first array is
assumed to contain the x-values for the following function values.


=head2 gnuplot_plot_equation

  $plot2->gnuplot_plot_equation(         # 3 equations in one plot
    "y1(x) = sin(x)",
    "y2(x) = cos(x)",
    "y3(x) = sin(x)/x" );

C<gnuplot_plot_equation> takes one or more B<gnuplot> function descriptions as
strings.  The plot ranges can be controlled by
L<C<gnuplot_set_xrange>|gnuplot_set_xrange> and
L<C<gnuplot_set_yrange>|gnuplot_set_yrange> .


=head2 gnuplot_pause


  $plot1->gnuplot_pause( [time] [,text] );

This is an emulation of the B<gnuplot> C<pause> command.  It displays any text
associated with the command and waits a specified amount of time or until the
carriage return is pressed.

C<time> may be any constant or expression. Choosing -1 (default) will wait
until a carriage return is hit, zero (0) won't pause at all, and a positive
number will wait the specified number of seconds.

The time value and the text are stored in the object and reused.  A sequence
like

  $plot1->gnuplot_pause( 5 );            # delay is 5 seconds
  $plot1->gnuplot_plot_y( \@y1 );

  $plot1->gnuplot_pause( );
  $plot1->gnuplot_plot_y( \@y2 );

  $plot1->gnuplot_pause( );
  $plot1->gnuplot_plot_y( \@y3 );

  $plot1->gnuplot_pause( -1 );

will display 3 plots with 5 seconds delay and wait for a final carriage return.


=head2 gnuplot_cmd

  $plot2->gnuplot_cmd( 'set grid',
                       'set timestamp "%d/%m/%y %H:%M" 0,0 "Helvetica"'
                       );

C<gnuplot_cmd> can be used to send one or more B<gnuplot> commands, especially
those not wrapped by a Graphics::GnuplotIF method.


=head3 Write a plot into a file

C<gnuplot_cmd> can be used to write a plot into a file or make a hardcopy by
setting/resetting the terminal and the output file:

  $plot1->gnuplot_cmd( 'set terminal png color',
                       'set output   "plot1.png" ' );

  $plot1->gnuplot_plot_xy( \@x, \@y1, \@y2 );

  $plot1->gnuplot_cmd( 'set output',
                       'set terminal x11' );

=head3 Make a hardcopy

A hardcopy can be made with an appropriate output format and a pipe to a
printer:

  $plot1->gnuplot_cmd( 'set terminal postscript',
                       'set output   " | lpr " ' );

  $plot1->gnuplot_plot_xy( \@x, \@y1, \@y2 );

  $plot1->gnuplot_cmd( 'set output',
                       'set terminal x11' );

=head2 gnuplot_reset

  $plot1->gnuplot_reset();

Set all options set with the C<set> command to their B<gnuplot> default values.



=head2 gnuplot_set_style

  $plot1->gnuplot_set_style( "steps" );   # new line style

Sets one of the allowed line styles (see L<new | new>)
in a plot command.


=head2 gnuplot_set_title

  $plot1->gnuplot_set_title("parabola");  # new title

Sets the plot title.
Equivalent to the B<gnuplot> command C<set title "parabola">.


=head2 gnuplot_set_xlabel

  $plot1->gnuplot_set_xlabel("time (days)");

Sets the x axis label.
Equivalent to the B<gnuplot> command C<set xlabel "time (days)">.


=head2 gnuplot_set_ylabel

  $plot1->gnuplot_set_ylabel("bugs fixed");

Sets the y axis label.
Equivalent to the B<gnuplot> command C<set ylabel "bugs fixed">.


=head2 gnuplot_set_xrange

  $plot1->gnuplot_set_xrange( left, right );

Sets the horizontal range that will be displayed.
Equivalent to the B<gnuplot> command C<set xrange [left:right]>.



=head2 gnuplot_set_yrange

  $plot1->gnuplot_set_yrange( low, high );

Sets the vertical range that will be displayed.
Equivalent to the B<gnuplot> command C<set yrange [low:high]>.


=head2 gnuplot_set_plot_titles

  $plot1->gnuplot_set_plot_titles( @ytitles );

Sets the list of titles used in the key for each of the y-coordinate data sets
specified in subsequent calls to gnuplot_plot_xy or gnuplot_plot_y commands.
This is not equivalent to a complete B<gnuplot> command; rather it adds a
C<title> clause to each data set specified in a B<gnuplot> C<plot> command.


=head1 DIAGNOSTICS

Dialog messages and diagnostic messages start with  C< Graphics::GnuplotIF (NR): ... > .

C<NR> is the number of the corresponding Graphics::GnuplotIF object and output
stream.  NR counts the objects in the order of their generation.


=head1 CONFIGURATION AND ENVIRONMENT

The environment variable DISPLAY is checked for the display.

=head1 DEPENDENCIES

=over 2

=item *
B<gnuplot> ( http://sourceforge.net/projects/gnuplot ) must be installed.

=item *
The module IO::Handle is used to handle output pipes.  Your operating system
must support pipes, of course.

=back

=head1 INCOMPATIBILITIES

There are no known incompatibilities.


=head1 BUGS AND LIMITATIONS

  $plot1->gnuplot_cmd("pause -1");     # send the gnuplot pause command

does not work. Use

  $plot1->gnuplot_pause( );

There are no known bugs in this module.  Please report problems to author.
Patches are welcome.


=head1 AUTHOR

Dr.-Ing. Fritz Mehner (mehner@fh-swf.de)


=head1 CREDITS

Stephen Marshall (smarshall@wsi.com) contributed C<gnuplot_set_plot_titles>.


=head1 LICENSE AND COPYRIGHT

Copyright (C) 2005-2007 by Fritz Mehner

This module is free software; you can redistribute it and/or modify it under
the same terms as Perl itself. See perldoc perlartistic.  This program is
distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.

=head1 SEE ALSO

C<gnuplot(1)>.

=cut

