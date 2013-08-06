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

var greenCross = {url: 'cross.png'};

function initialize() {
        
var mapOptions = {
	center: new google.maps.LatLng(-31.980569, 115.817807),
	zoom: 17,
	mapTypeId: google.maps.MapTypeId.SATELLITE
        };

map = new google.maps.Map(document.getElementById("map-canvas"), mapOptions);

google.maps.event.addListener(map, 'click', function(event) { logLocation(event.latLng); });

}


function logLocation(location) {
 

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


function updateMarkersText() {

var markersString = "";

for(i in markers) {

markersString += i + "," + markers[i].getPosition().lat() + "," + markers[i].getPosition().lng() + "\\n";

markers[i].setTitle(i.toString());

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

logLocation(position);

}

}


function clearMap() {

document.getElementById("nextposn").value = 0;

while(markers.length > 0) {
	for(i in markers) {
		removeMarker(markers[i]);
	}
}

}

function removeMarker(marker) { 

marker.setMap(null);

var index = markers.indexOf(marker);

markers.splice(index,1);

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

                            }
		,error: function() { alert("AJAX Error!"); }
		});

		setTimeout(updateLog,1000);

}


    </script>
  </head>
  <body>


<table border="1" width="100%">

<tr><td>

<form method="POST" action="waypoints.cgi">

Saved maps: <select name="openfilename">

END

opendir(DIR,"../maps/");
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

<br /><br />
Next Position: <input type="text" id="nextposn" size="4" value="0" />
<input type="button" onclick="clearMap()" value="Clear" /> <input type="button" onclick="updateMarkersFromText()" value="Update Map" /> 
<br />
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

    <td height="600" width="75%"><div id="map-canvas"></div></td></tr>
<tr><td colspan="2"><textarea rows="15" cols="100" id="logarea"></textarea></td></tr>
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
