#!/usr/bin/perl

use strict;

use CGI qw/:standard/;
use CGI::Carp qw(fatalsToBrowser warningsToBrowser);

use Math::Trig;

my $TwoPi = 4*acos(0);
my $EARTH_RADIUS = 6371000;


sub LatLongToXY {

	my($lat,$lng,$DatumLat,$DatumLong) = @_;

	my @Result;

	$Result[1] = -1*$EARTH_RADIUS*$TwoPi*($DatumLat - $lat)/360;
	$Result[0] = -1*$EARTH_RADIUS*cos(abs($lat))*$TwoPi*($DatumLong - $lng)/360;

	return @Result;

}

sub XYToLatLong {

	my($x,$y,$DatumLat,$DatumLong) = @_;

	my @Result;

	$Result[0] = 1*$y*360/($EARTH_RADIUS*$TwoPi) + $DatumLat;
	$Result[1] = 1*$x*360/($EARTH_RADIUS*$TwoPi*cos(abs($Result[0]))) + $DatumLong;

	return @Result;

}

print header;
warningsToBrowser(1);

my $Error;

my $Data;

my $CurrentName;

if(param("save") eq 'Save') {

	if(param("mapname") =~ /^[0-9a-z_]+$/i) {

		$CurrentName = param("mapname") . ".wyp";

		my ($DatumLat, $DatumLong);

		open (FILE,">./maps/" . $CurrentName);

		my @MarkersText = split(/\n/,param("markerstext"));
		
		foreach my $Line (@MarkersText) {

			my @LineParts = split(/,/, $Line);

			if($LineParts[0] eq 'D') { 
				$DatumLat = $LineParts[1]; 
				$DatumLong = $LineParts[2]; 
				print FILE $Line . "\n";			
			}
			else { next; }

		}

		foreach my $Line (@MarkersText) {

			my @LineParts = split(/,/, $Line);

			if($LineParts[0] eq 'D') { next; }
			else { 
				my @Point = LatLongToXY($LineParts[1], $LineParts[2], $DatumLat, $DatumLong);
				print FILE $LineParts[0] . "," . $Point[0] . "," . $Point[1] . "\n";
			}

		}



		close FILE;

	}

	else { $Error = "Bad map name"; }

}

if(param("open") eq 'Open' || param("save") eq 'Save') {

	if(param("open") eq 'Open') { $CurrentName = param("openfilename"); }

	open (FILE,"./maps/$CurrentName");
	my @lines = <FILE>;
	
	my ($DatumLat, $DatumLong);

	foreach my $line (@lines) {

		my @LineParts = split(/,/, $line);

		if($LineParts[0] eq 'D') { 
			$DatumLat = $LineParts[1]; 
			$DatumLong = $LineParts[2]; 	
		}
		else { next; }

	}

	foreach my $line (@lines) {

		$line =~ s/\n//g;
		my @LineParts = split(/,/, $line);

		if($LineParts[0] eq 'D') { $Data .= $line . "\n"; }
		else { 
			
			my ($Lat, $Long) = XYToLatLong($LineParts[1],$LineParts[2],$DatumLat,$DatumLong);
			$Data .= $LineParts[0] . "," . $Lat . "," . $Long . "\n";

		}

	}


	close FILE;

}




print <<END;

<!DOCTYPE html>
<html>
  <head>
	<title>Waypoint Recorder</title>
    <meta name="viewport" content="initial-scale=1.0, user-scalable=no" />
	<meta http-equiv="Content-Type" content="text/html;charset=utf-8" />
    <style type="text/css">
      #map-canvas { height: 100% }
    </style>
    <script type="text/javascript"
      src="https://maps.googleapis.com/maps/api/js?key=AIzaSyBUx74Zh1SWTbx4wxEjyWCxjJg4MaA8FUI&amp;sensor=true">
    </script>	
<script type="text/javascript" src="jquery-2.0.3.js">
  	</script>
    <script src="RGraph/libraries/RGraph.common.core.js" ></script>
    <script src="RGraph/libraries/RGraph.scatter_improved.js" ></script>
    <script src="RGraph/libraries/RGraph.common.dynamic.js" ></script>
    <script src="RGraph/libraries/RGraph.common.tooltips.js" ></script>
    <script type="text/javascript">

window.onload = function ()
        {
        	setTimeout(updateLog,1000);
		
        }

var map;

var markers = [];
var fencemarkers = [];

var offsetLat = 0;
var offsetLong = 0;

var greenCross = {url: 'cross.png'};
var blueDot = {url: 'blue-dot.png'};

var redCross = {url: 'redcross.png'};

var currentLocationMarker;
var datumMarker;

function initialize() {

	var defaultPos = new google.maps.LatLng(-31.980569, 115.817807);
        
	var mapOptions = {
		center: defaultPos,
		zoom: 17,
		mapTypeId: google.maps.MapTypeId.SATELLITE
        };

	map = new google.maps.Map(document.getElementById("map-canvas"), mapOptions);

	google.maps.event.addListener(map, 'click', function(event) { logLocation(event.latLng); });

	var zeroposition = new google.maps.LatLng(0, 0);

	currentLocationMarker = new google.maps.Marker({
		position: zeroposition,
		map: map,
		raiseOnDrag: false,
		icon: blueDot,
	});

	datumMarker = new google.maps.Marker({
		position: zeroposition,
		map: map,
		raiseOnDrag: false,
	});

	if(document.getElementById("markerstext").value != "") { updateMarkersFromText(); }
	else { setDatumLocation(defaultPos); }

}

function logLocation(location) {

	if(document.getElementById("markertype").options[document.getElementById("markertype").selectedIndex].value == 0) {
		logMarkerLocation(location);
	}
	else if(document.getElementById("markertype").options[document.getElementById("markertype").selectedIndex].value == 3) {
		setDatumLocation(location);
	}
	else { logFenceLocation(location); }

}


function logMarkerLocation(location) {
 

	var marker = new google.maps.Marker({
		position: location,
		map: map,
		draggable: true,
		raiseOnDrag: false,
		icon: greenCross,
	});

	google.maps.event.addListener(marker, 'rightclick',  function() { removeMarker(marker); });

	markers.splice(document.getElementById("nextposn").value,0,marker);

	document.getElementById("nextposn").value++;

	updateMarkersText();

}

function logFenceLocation(location) {

	var circleOptions = {
      		strokeColor: '#FF0000',
      		strokeOpacity: 0.8,
      		strokeWeight: 2,
      		fillColor: '#FF0000',
      		fillOpacity: 0.35,
      		map: map,
      		center: location,
      		radius: 1.5
    	};

	var fenceCircle = new google.maps.Circle(circleOptions);

	google.maps.event.addListener(fenceCircle, 'rightclick',  function() { removeFenceMarker(fenceCircle); });

	fencemarkers.push(fenceCircle);

	updateMarkersText();

}

function setDatumLocation(location) {

	datumMarker.setPosition(location);

	updateMarkersText();

}


function updateMarkersText() {

	var markersString = "D," + (datumMarker.getPosition().lat() - offsetLat) + "," + (datumMarker.getPosition().lng() - offsetLong) + "\\n";

	for(i in markers) {

		markersString += i + "," + (markers[i].getPosition().lat() - offsetLat) + "," + (markers[i].getPosition().lng() - offsetLong) + "\\n";

		markers[i].setTitle(i.toString());

	}

	for(i in fencemarkers) {

		markersString += "F" + i + "," + (fencemarkers[i].getCenter().lat() - offsetLat) + "," + (fencemarkers[i].getCenter().lng() - offsetLong) + "\\n";

	}



	document.getElementById("markerstext").value = markersString;

}

function updateMarkersFromText() {

	var lines = document.getElementById("markerstext").value.split("\\n");

	clearMap();

	for(i in lines) {

		if(lines[i] == '') { continue; }

		var lineParts = lines[i].split(",");

		var position = new google.maps.LatLng(parseFloat(lineParts[1]) + offsetLat,parseFloat(lineParts[2]) + offsetLong);

		if(lineParts[0].substring(0,1) == "F") { logFenceLocation(position); }
		else if(lineParts[0].substring(0,1) == "D") { setDatumLocation(position); }
		else { logMarkerLocation(position); }

	}

}


function clearMap() {

	document.getElementById("nextposn").value = 0;

	while(markers.length > 0) {
		for(i in markers) {
			removeMarker(markers[i]);
		}
	}

	while(fencemarkers.length > 0) {
		for(i in fencemarkers) {
			removeFenceMarker(fencemarkers[i]);
		}
	}

}

function removeMarker(marker) { 

	marker.setMap(null);

	var index = markers.indexOf(marker);

	markers.splice(index,1);

	updateMarkersText();
	
}

function removeFenceMarker(marker) { 

	marker.setMap(null);

	var index = fencemarkers.indexOf(marker);

	fencemarkers.splice(index,1);

	updateMarkersText();
	
}




google.maps.event.addDomListener(window, 'load', initialize);

function updateLog() {

	\$.ajax({
		type: "GET"
		,url: "getcarinfo.cgi"
		,dataType: "json"
		,cache: false
		,success: function(json){

				document.getElementById("logarea").innerHTML = json.log;

				document.getElementById("paramarea").innerHTML = "";

				var msgstrings = new Array(35).join('0').split('');

        			for(var key in json.params) {
         				var attrName = key;
            				var attrValue = json.params[key].content;
					msgstrings[json.params[key].order] = "<b>" + attrName + "</b>: " + attrValue + "<br />";
					
        			}

				document.getElementById("paramarea").innerHTML = msgstrings.join('');				

				var position = new google.maps.LatLng(parseFloat(json.gps.lat) - offsetLat, parseFloat(json.gps.long) - offsetLong);
				currentLocationMarker.setPosition(position);

				drawXYGraph(json.mapdata);

                            }
		,error: function() { alert("AJAX Error!"); }
	});

	setTimeout(updateLog,1000);

}

function loadMap() {

	\$.ajax({
		type: "POST",
		url: "sendcommand.cgi",
		data: "command=LOADMAP," + document.getElementById("openfilename").options[document.getElementById("openfilename").selectedIndex].value,
		success: function() { },
		dataType: "text",
		error: function() { alert("AJAX IPC send Error!"); }
	});


}

function setGPSOffset() {

	\$.ajax({
		type: "POST",
		url: "sendcommand.cgi",
		data: "command=SETGPSOFF," + document.getElementById("latoffset").value + "," + document.getElementById("longoffset").value,
		success: function() { },
		dataType: "text",
		error: function() { alert("AJAX IPC send Error!"); }
	});

}


function setMapOffset() {

	offsetLat = parseFloat(document.getElementById("maplatoffset").value);
	offsetLong = parseFloat(document.getElementById("maplongoffset").value);

	updateMarkersFromText();

}

function addCurPos() {

	logLocation(currentLocationMarker.getPosition());

}

function sendCommand(Command) {

	\$.ajax({
		type: "POST",
		url: "sendcommand.cgi",
		data: "command=" + Command,
		success: function() { },
		dataType: "text",
		error: function() { alert("AJAX IPC send Error!"); }
	});


}


function drawXYGraph(json)
        {

		RGraph.Reset(document.getElementById('cvs'));

            var scatter = new RGraph.Scatter('cvs', json)
		.Set('scale.decimals', 1)
		.Set('xscale.decimals', 1)
                .Set('xscale', true)
		.Set('chart.gutter.left', 60)
                .Draw();

        }


    </script>
  </head>
  <body>

<form method="POST" action="waypoints.cgi">
<table border="1" width="100%">
<tr><td colspan="2" style="font-family: Arial; text-align: center; font-size: 18px">UWA Autonomous SAE Car Web Interface</td></tr>
<tr><td>

<div style="font-family: Arial; font-size: 16px">Mapping Tools: </div>

Saved maps: <select id="openfilename" name="openfilename">

END

opendir(DIR,"./maps/");
my @Files = readdir(DIR);
close DIR;

foreach my $File (@Files) {

	if($File =~ /^\.+$/) { next; }

	else { 
		if($File eq $CurrentName) { print "<option selected=\"selected\" value=\"$File\">$File</option>"; }
		else { print "<option value=\"$File\">$File</option>"; }
	}

}

print <<END;

</select>

<input type="submit" name="open" value="Open" />
<input type="button" onclick="loadMap()" name="load" value="Load into Control" />

<br /><br />
Next Position: <input type="text" id="nextposn" size="4" value="0" />
<input type="button" onclick="clearMap()" value="Clear" /> <input type="button" onclick="updateMarkersFromText()" value="Update Map" /> 
<br />
Type to place: <select id="markertype"><option selected="selected" value="0">Waypoint</option><option value="1">Fence Post</option><option value="3">Datum</option></select>

<input type="button" value="Add current pos" onclick="addCurPos()" />

<textarea name="markerstext" id="markerstext" rows="15" cols="45">
END

print $Data;

$CurrentName =~ s/\.wyp//;

print <<END; 
</textarea> 

<br />

Save map as: <input type="text" size="20" name="mapname" value="$CurrentName" />

<input type="submit" name="save" value="Save" />
<br /><br />

<input type="text" size="6" id="maplatoffset" name="maplatoffset" value="0.0" />
<input type="text" size="6" id="maplongoffset" name="maplongoffset" value="0.0" />
<input type="button" onclick="setMapOffset()" name="start" value="Set Map Offset" />


</td>

    <td height="500" width="65%" rowspan="2"><div id="map-canvas"></div></td></tr>

<tr><td>
<div style="font-family: Arial; font-size: 16px">Car Commands: </div>
<input type="button" onclick="sendCommand('AUTOSTART')" name="start" value="Start Auto" />
<input type="button" onclick="sendCommand('AUTOSTOP')" name="stop" value="STOP Auto" />
<input type="button" onclick="sendCommand('AUTOPAUSE')" name="pause" value="Pause Auto" />
<input type="button" onclick="sendCommand('AUTOCONT')" name="cont" value="Continue Auto" /> <br /><br />


<input type="button" onclick="sendCommand('STARTREC')" name="pause" value="Start Recording" />
Map Name: <input type="text" size="6" id="mapname" name="mapname" value="" />
<input type="button" onclick="sendCommand('STOPREC,' + document.getElementById('mapname').value)" name="cont" value="Finish Recording" /> <br /><br />


<input type="button" onclick="sendCommand('TOGBIL')" name="cont" value="Toggle BrakeIL" /> 
<input type="button" onclick="sendCommand('SETDATUM,' + (datumMarker.getPosition().lat() - offsetLat) + ',' + (datumMarker.getPosition().lng() - offsetLong))" name="cont" value="Set Datum" /> 
<input style="background-color : red;" type="button" onclick="sendCommand('ESTOP')" name="estop" value="ESTOP Car" />
<br /><br />
<input type="text" size="6" id="latoffset" name="latoffset" value="0.0" />
<input type="text" size="6" id="longoffset" name="longoffset" value="0.0" />
<input type="button" onclick="setGPSOffset()" name="start" value="Set GPS Offset" />



<tr><td colspan="2"><textarea rows="20" cols="60" id="logarea"></textarea><div style="float: left; height: 20em; width: 25em; overflow: auto; border: 1px solid black" id="paramarea"></div></td></tr>
</table>

</form>


    <canvas id="cvs" width="600" height="600">[No canvas support]</canvas>

END

if($Error) { 
print <<END;

<script type="text/javascript">alert("$Error");</script>

END

}



print <<END;

  </body>
</html>

END
