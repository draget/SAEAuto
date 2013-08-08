#!/usr/bin/perl

use strict;

use CGI qw/:standard/;
use CGI::Carp qw(fatalsToBrowser warningsToBrowser);

my $Error;

my $Data;

my $CurrentName;

if(param("save") eq 'Save') {

	if(param("mapname") =~ /^[0-9a-z_]+$/i) {

		$CurrentName = param("mapname") . ".wyp";

		open (FILE,">./maps/" . $CurrentName);
		print FILE param("markerstext");
		close FILE;

	}

	else { $Error = "Bad map name"; }

	$Data = param("markerstext");

}

elsif(param("open") eq 'Open') {

	my $Filename = param("openfilename");

	$CurrentName = param("openfilename");

	open (FILE,"./maps/$Filename");
	my @lines = <FILE>;
	$Data = join("",@lines);
	close FILE;

}


print header;
warningsToBrowser(1);

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
    <script type="text/javascript">

window.onload = function ()
        {
        	setTimeout(updateLog,1000);
		
        }

var map;

var markers = [];
var fencemarkers = [];

var greenCross = {url: 'cross.png'};
var blueDot = {url: 'blue-dot.png'};

var redCross = {url: 'redcross.png'};

var currentLocationMarker;
var datumMarker;

function initialize() {
        
	var mapOptions = {
		center: new google.maps.LatLng(-31.980569, 115.817807),
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
	
	\$.ajax({
		type: "POST",
		url: "sendcommand.cgi",
		data: "command=SETDATUM," + location.lat() + "," + location.lng(),
		success: function() { },
		dataType: "text",
		error: function() { alert("AJAX IPC send Error!"); }
	});

}


function updateMarkersText() {

	var markersString = "";

	for(i in markers) {

		markersString += i + "," + markers[i].getPosition().lat() + "," + markers[i].getPosition().lng() + "\\n";

		markers[i].setTitle(i.toString());

	}

	for(i in fencemarkers) {

		markersString += "F" + i + "," + fencemarkers[i].getCenter().lat() + "," + fencemarkers[i].getCenter().lng() + "\\n";

	}



	document.getElementById("markerstext").value = markersString;

}

function updateMarkersFromText() {

	var lines = document.getElementById("markerstext").value.split("\\n");

	clearMap();

	for(i in lines) {

		if(lines[i] == '') { continue; }

		var lineParts = lines[i].split(",");

		var position = new google.maps.LatLng(lineParts[1],lineParts[2]);

		if(lineParts[0].substring(0,1) == "F") { logFenceLocation(position); }
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
				document.getElementById("paramarea").innerHTML = json.params;
				var position = new google.maps.LatLng(json.gps.lat, json.gps.long);
				currentLocationMarker.setPosition(position);

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


    </script>
  </head>
  <body>


<table border="1" width="100%">
<tr><td colspan="2" style="font-family: Arial; text-align: center; font-size: 18px">UWA Autonomous SAE Car Web Interface</td></tr>
<tr><td>

<form method="POST" action="waypoints.cgi">

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
<textarea name="markerstext" id="markerstext" rows="15" cols="45">
END

print $Data;

$CurrentName =~ s/\.wyp//;

print <<END; 
</textarea> 

<br />

Save map as: <input type="text" size="20" name="mapname" value="$CurrentName" /> <br />

<input type="submit" name="save" value="Save" />

</form>

</td>

    <td height="600" width="75%" rowspan="2"><div id="map-canvas"></div></td></tr>

<tr><td>

<input type="button" onclick="sendCommand('AUTOSTART')" name="start" value="Start Auto" />
<input type="button" onclick="sendCommand('AUTOSTOP')" name="stop" value="STOP Auto" />
<input type="button" onclick="sendCommand('AUTOPAUSE')" name="pause" value="Pause Auto" />
<input type="button" onclick="sendCommand('AUTOCONT')" name="cont" value="Continue Auto" />
<input style="background-color : red;" type="button" onclick="sendCommand('ESTOP')" name="estop" value="ESTOP Car" />
<br />
<input type="text" id="latoffset" value="0.0" />
<input type="text" id="longoffset" value="0.0" />
<input type="button" onclick="setGPSOffset()" name="start" value="Set GPS Offset" />

<tr><td colspan="2"><textarea rows="20" cols="60" id="logarea"></textarea><div style="float: left; height: 20em; width: 25em; overflow: auto; border: 1px solid black" id="paramarea"></div></td></tr>
</table>

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
